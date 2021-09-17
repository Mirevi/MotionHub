#include "OsgQtWidget.h"
#include "MotionHubUtil/ConfigManager.h"
#include "TrackerManagement/TrackerManager.h"
#include "OsgGrid.h"

#include <osg/PositionAttitudeTransform>

#include <QVBoxLayout>

#include "OsgLine.h"
#include "OsgBone.h"
#include "OsgSkeleton.h"


namespace osgQt
{
	class GraphicsWindowQt;
}

OsgQtWidget::OsgQtWidget(osgQt::GraphicsWindowQt* gw, TrackerManager* trackerManager, ConfigManager* configManager) :
	QWidget(), m_refTrackerManager(trackerManager), m_configManager(configManager)
{
	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	osg::Camera* camera = m_viewer.getCamera();
	camera->setGraphicsContext(gw);
	camera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 0.1f, 1000.0f);



	//osg::Node* scene = osgDB::readNodeFile("cow.osg");
	m_sceneRoot = new osg::Group();
	m_sceneRoot->setName("m_sceneRoot");
	m_viewer.setSceneData(m_sceneRoot);
	m_viewer.addEventHandler(new osgViewer::StatsHandler);
	m_cameraManipulator = new osgGA::TrackballManipulator();

	m_cameraManipulator->setVerticalAxisFixed(false);

	camera->setViewMatrixAsLookAt(osg::Vec3f(0.0, 1.5, 6.0), osg::Vec3f(0.0, 1.0, 0.0), osg::Vec3f(0.0, 1.0, 0.0));

	m_cameraManipulator->setHomePosition(osg::Vec3f(0.0, 1.5, 6.0), osg::Vec3f(0.0, 1.0, 0.0), osg::Vec3f(0.0, 1.0, 0.0), false);
	m_viewer.setCameraManipulator(m_cameraManipulator);
	m_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(gw->getGLWidget());
	setLayout(layout);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(10); //TODO1: Hardcoded fps timing

	// setup grid
	int lineCountGrid = 12;
	m_configManager->readInt("line_count_for_floor_grid", lineCountGrid);

	float cellSizeGrid = 0.5;
	m_configManager->readFloat("cell_size_for_floor_grid", cellSizeGrid);

	float lineWidthForRGBAxesGrid = 2.0f;
	m_configManager->readFloat("line_width_for_rgb_axes_floor_grid", lineWidthForRGBAxesGrid);

	float lineWidthForGreyAxesGrid = 1.0f;
	//m_configManager->readFloat("line_width_for_grey_axes_floor_grid", lineWidthForGreyAxesGrid);

	OsgGrid grid(lineCountGrid, OsgGrid::SHOW_X_Z, cellSizeGrid, lineWidthForRGBAxesGrid, lineWidthForGreyAxesGrid);
	grid.attachToSceneGraph(m_sceneRoot);

	//setup confidence colors
	m_colorRed = osg::Vec4f(0.75f, 0.0f, 0.0f, 1.0f);
	m_colorYellow = osg::Vec4f(0.75f, 0.75f, 0.0f, 1.0f);
	m_colorGreen = osg::Vec4f(0.0f, 0.75f, 0.0f, 1.0f);
}

//Is envoked, when no (global) tracking is active and a new tracker is added or removed
void OsgQtWidget::updateSkeletonMeshPoolSize()
{

	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	// lock the tracker pool
	m_refTrackerManager->getTrackerPoolLock()->lock();

	// get the trackerRefPoolSize
	int trackerRefPoolSize = m_skeletonMeshPool.size();
	// get the trackerPoolSize
	int trackerPoolSize = trackerTempCopy.size();

	// add new item to the pool if the trackerRefPoolSize is smaller than the trackerPoolSize
	if (trackerRefPoolSize < trackerPoolSize)
	{
		// loop over all tracker in the tracker pool in order to find the missing one
		for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
		{
			// if the current tracker is not a referenced item in m_skeletonMeshPool, then add a new one
			if (m_skeletonMeshPool.find((*itTracker)->getProperties()->id) == m_skeletonMeshPool.end())
			{

				Console::log("GlWidget::updateSkeletonMeshPoolSize(): insert");

				// add the tracker reference id as a key and create an new SkeletonMesh vector
				m_skeletonMeshPool.insert(std::make_pair((*itTracker)->getProperties()->id, std::vector<OsgSkeleton>()));

			}
		}
	}

	// remove item from the pool if the trackerRefPoolSize is bigger than the trackerPoolSize
	else if (trackerRefPoolSize > trackerPoolSize)
	{
		// remove items till the m_skeletonMeshPool size is same as the trackerTempCopy size
		while (m_skeletonMeshPool.size() > trackerTempCopy.size())
		{

			// loop over all m_skeletonMeshPool items and remove the one which can't be found in trackerTempCopy
			for (auto itRefTracker = m_skeletonMeshPool.begin(); itRefTracker != m_skeletonMeshPool.end(); itRefTracker++)
			{
				bool isTrackerInPool = false;

				for (int indexTrackerCopy = 0; indexTrackerCopy < trackerTempCopy.size(); indexTrackerCopy++)
				{

					if (trackerTempCopy.at(indexTrackerCopy)->getProperties()->id == itRefTracker->first)
					{

						isTrackerInPool = true;
						break;

					}
				}

				if (isTrackerInPool == false)
				{
					for (int i = 0; i < m_skeletonMeshPool.at(itRefTracker->first).size(); i++)
					{
						m_skeletonMeshPool.at(itRefTracker->first).at(i).removeAndDelete();
					}

					m_skeletonMeshPool.erase(itRefTracker->first);
					break;

				}
			}
		}
	}
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}


//Is envoked, when (global) tracking is active and a new skeleton of a tracker (for example spawned by Azure Kinect) appears in the tracking data
void OsgQtWidget::updateSkeletonMeshCount()
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	// lock the tracker pool
	m_refTrackerManager->getTrackerPoolLock()->lock();

	// loop over all tracker
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		// if tracker is active update
		if ((*itTracker)->isTracking())
		{

			// get skeletonPoolSize
			int skeletonPoolSize = skeletonPoolTempCopy.size();
			// get skeletonMeshPoolSize by id of current tracker
			int skeletonMeshPoolSize = m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size();

			// add skeleton mesh to skeleton mesh pool when skeletonMeshPoolSize is smaller than skeletonPoolSize
			if (skeletonMeshPoolSize < skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() < skeletonPoolTempCopy.size())
				{
					// add new skeletonMesh to skeltonMeshPool
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.push_back(OsgSkeleton(m_sceneRoot));

				}
			}
			// remove skeleton mesh from skeleton mesh pool when skeletonMeshPoolSize is bigger than skeletonPoolSize
			else if (skeletonMeshPoolSize > skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() > skeletonPoolTempCopy.size())
				{
					// remove skeletonMesh from skeletonMeshPool
					for (int i = 0; i < m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size(); i++)
					{
						m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(i).removeAndDelete();
					}
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.pop_back();

				}
			}
		}
	}

	// unlock the trackerPool
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}


// Loop every MotionHub GLWindow frame
void OsgQtWidget::updateSkeletonMeshTransform()
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		// update skeleton joint position and rotation if new data is available
		if ((*itTracker)->isTracking() && (*itTracker)->isDataAvailable())
		{

			// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
			std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

			int indexSkeleton = 0;

			// update each skeleton
			for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
			{

				//Update skeleton
				m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(indexSkeleton).update(itSkeleton->second);

				int indexJoint = 0;

				// update each joint
				for (auto itJoint = itSkeleton->second.m_joints.begin(); itJoint != itSkeleton->second.m_joints.end(); itJoint++)
				{

					//Console::log("GlWidget::updateSkeletonMeshTransform(): pool size = " + toString(m_skeletonMeshPool.size()));

					// get current skeleton
					auto currMeshTracker = m_skeletonMeshPool.find((*itTracker)->getProperties()->id);

					if (currMeshTracker->second.size() == 0)
					{
						return;
					}

					// TODO2: set joint confidence in the shader
					switch (itJoint->second.getJointConfidence())
					{

					case Joint::HIGH:
						//currJoint->setDiffuseColor(m_colorGreen);
						//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorGreen.m_xyz.x, m_colorGreen.m_xyz.y, m_colorGreen.m_xyz.z, 1.0f));
						break;

					case Joint::MEDIUM:
						//currJoint->setDiffuseColor(m_colorYellow);
						//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorYellow.m_xyz.x, m_colorYellow.m_xyz.y, m_colorYellow.m_xyz.z, 1.0f));
						break;

					case Joint::LOW:
						//currJoint->setDiffuseColor(m_colorRed);
						//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorRed.m_xyz.x, m_colorRed.m_xyz.y, m_colorRed.m_xyz.z, 1.0f));
						break;

					case Joint::NONE:
						//currJoint->setDiffuseColor(Vector3::one());
						//m_spheres.at(indexJoint)->setColor(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
						break;

					default:
						break;

					}

					indexJoint++;

				}

				indexSkeleton++;

			}
		}
	}
}


void OsgQtWidget::drawLine(const osg::Vec3 start, const osg::Vec3 end, const osg::Vec4 colorStart, const osg::Vec4 colorEnd) {
	//m_vertices->push_back(start);
	//m_vertices->push_back(end);
	//m_colors->push_back(colorStart);
	//m_colors->push_back(colorEnd);
	////RedrawLines();
	//m_isDirty = true;
}

void OsgQtWidget::setCameraTransform(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up)
{
	m_viewer.getCamera()->setViewMatrixAsLookAt(eye, center, up);
}

void OsgQtWidget::mouseMoveEvent(QMouseEvent* event)
{
	QPoint globalCursorPos = QCursor::pos();

	emit osgWidgetMoved(osg::Vec2(globalCursorPos.x(), globalCursorPos.y()));

	QWidget::mouseMoveEvent(event);

}

void OsgQtWidget::mousePressEvent(QMouseEvent* event)
{
	QPoint globalCursorPos = QCursor::pos();

	//Console::log("MainWindow::slotOsgWidgetPressed(): " + toString(globalCursorPos.x()) + ", " + toString(globalCursorPos.y()));
	Console::log("MainWindow::slotOsgWidgetPressed()");

	emit osgWidgetPressed(osg::Vec2(globalCursorPos.x(), globalCursorPos.y()));

	QWidget::mousePressEvent(event);

}

void OsgQtWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QPoint globalCursorPos = QCursor::pos();

	emit osgWidgetReleased(osg::Vec2(globalCursorPos.x(), globalCursorPos.y()));

	QWidget::mouseReleaseEvent(event);

}


void OsgQtWidget::toggleJointAxes(bool menuValue)
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{
		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		int indexSkeleton = 0;

		// update each skeleton
		for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
		{

			//Toggle RGB joint axes for each skeleton
			m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(indexSkeleton).toggleJointAxes(menuValue);

			indexSkeleton++;

		}
	}
}

void OsgQtWidget::toggleStickManRendering(bool menuValue)
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{
		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		int indexSkeleton = 0;

		// update each skeleton
		for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
		{

			//Toggle RGB joint axes for each skeleton
			m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(indexSkeleton).toggleStickManRendering(menuValue);

			indexSkeleton++;

		}
	}
}


void OsgQtWidget::toggleSolidBoneRendering(bool menuValue)
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{
		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		int indexSkeleton = 0;

		// update each skeleton
		for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
		{

			//Toggle RGB joint axes for each skeleton
			m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(indexSkeleton).toggleSolidBoneRendering(menuValue);

			indexSkeleton++;

		}
	}
}


void OsgQtWidget::toggleConfidenceSpheresRendering(bool menuValue)
{
	
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{
		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		int indexSkeleton = 0;

		// update each skeleton
		for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
		{

			//Toggle RGB joint axes for each skeleton
			m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.at(indexSkeleton).toggleConfidenceSpheresRendering(menuValue);

			indexSkeleton++;

		}
	}

}



