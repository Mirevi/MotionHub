#include "OsgQtWidget.h"
#include "MotionHubUtil/ConfigManager.h"
#include "TrackerManagement/TrackerManager.h"
#include "Grid.h"

#include <QVBoxLayout>



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
	m_cameraManipulator->setHomePosition(osg::Vec3f(6.0, 1.5, 0.0), osg::Vec3f(0.0, 1.0, 0.0), osg::Vec3f(0.0, 1.0, 0.0), false);
	m_viewer.setCameraManipulator(m_cameraManipulator);
	m_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(gw->getGLWidget());
	setLayout(layout);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
	m_timer.start(10); //TODO1: Hardcoded fps timing


	int lineCountGrid = 20;
	m_configManager->readInt("line_count_for_floor_grid", lineCountGrid);
	
	float cellSizeGrid = 5.0;
	m_configManager->readFloat("line_width_for_rgb_axes_floor_grid", cellSizeGrid);

	float lineWidthForRGBAxesGrid = 2.0;
	m_configManager->readFloat("line_width_for_grey_axes_floor_grid", lineWidthForRGBAxesGrid);

	float lineWidthForGreyAxesGrid = 0.1;
	m_configManager->readFloat("cell_size_for_floor_grid", lineWidthForGreyAxesGrid);

	
	
	Grid grid(lineCountGrid, SHOW_X_Z, cellSizeGrid, lineWidthForRGBAxesGrid, lineWidthForGreyAxesGrid);
	grid.attachToSceneGraph(m_sceneRoot);


	m_colorRed = Vector3(0.75f, 0.0f, 0.0f);
	m_colorYellow = Vector3(0.75f, 0.75f, 0.0f);
	m_colorGreen = Vector3(0.0f, 0.75f, 0.0f);

	for (int i = 0; i <= 20; i++)
	{
		m_spheres.push_back(new osg::ShapeDrawable());
		m_spheres.at(i)->setShape(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.035f));
		m_spheres.at(i)->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		m_sphereTransforms.push_back(new osg::MatrixTransform());
		m_sphereTransforms.at(i)->addChild(m_spheres.at(i));
		m_sceneRoot->addChild(m_sphereTransforms.at(i));
	}
}


void OsgQtWidget::updateSkeletonMeshPoolSize()
{
	std::cout << "Test aus updateSkeletonMeshPoolSize" << std::endl;
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
			// if the current tracker is not an refferenced item in m_skeletonMeshPool add a new one
			if (m_skeletonMeshPool.find((*itTracker)->getProperties()->id) == m_skeletonMeshPool.end())
			{

				Console::log("GlWidget::updateSkeletonMeshPoolSize(): insert");

				// add the tracker refference id as a key and create an new SkeletonMesh vector
				m_skeletonMeshPool.insert(std::make_pair((*itTracker)->getProperties()->id, std::vector<SkeletonMesh>()));

			}
		}
	}

	// remove item from the pool if the trackerRefPoolSize is bigger than the trackerPoolSize
	else if (trackerRefPoolSize > trackerPoolSize)
	{
		// remove items till the m_skeletonMeshPool size is same as the trackerTempCopy size
		while (m_skeletonMeshPool.size() > trackerTempCopy.size())
		{

			// loop over all m_skeletonMeshPool items and remove the one which cant be found in trackerTempCopy
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

					m_skeletonMeshPool.erase(itRefTracker->first);
					break;

				}
			}
		}
	}
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}


void OsgQtWidget::updateSkeletonMeshTransform()
{
	//std::cout << "Test aus updateSkeletonMeshTransform" << std::endl;
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

					// get current joint
					//Cube* currJoint = currMeshTracker->second.at(indexSkeleton).m_joints[indexJoint];

					// set joint position and rotation
					//currJoint->setPosition(itJoint->second.getJointPosition());
					//currJoint->setRotation(itJoint->second.getJointRotation());

					//HWM: Ist eine geometry, und die hänge ich an eine Matrix bzw. PosAtt und diese hänge ich an den scenengraph.
					osg::Matrix transformMatrix;
					transformMatrix = osg::Matrix::rotate(osg::Quat(itJoint->second.getJointRotation().x(), 
																	itJoint->second.getJointRotation().y(), 
																	itJoint->second.getJointRotation().z(), 
																	itJoint->second.getJointRotation().w()))
									* osg::Matrix::translate(osg::Vec3f(itJoint->second.getJointPosition().x(),
																		itJoint->second.getJointPosition().y(),
																		itJoint->second.getJointPosition().z()));
					m_sphereTransforms.at(indexJoint)->setMatrix(transformMatrix);

					// set joint confidence in the shader
					switch (itJoint->second.getJointConfidence())
					{

					case Joint::HIGH:
						//currJoint->setDiffuseColor(m_colorGreen);
						m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorGreen.m_xyz.x, m_colorGreen.m_xyz.y, m_colorGreen.m_xyz.z, 1.0f));
						break;

					case Joint::MEDIUM:
						//currJoint->setDiffuseColor(m_colorYellow);
						m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorYellow.m_xyz.x, m_colorYellow.m_xyz.y, m_colorYellow.m_xyz.z, 1.0f));
						break;

					case Joint::LOW:
						//currJoint->setDiffuseColor(m_colorRed);
						m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorRed.m_xyz.x, m_colorRed.m_xyz.y, m_colorRed.m_xyz.z, 1.0f));
						break;

					case Joint::NONE:
						//currJoint->setDiffuseColor(Vector3::one());
						m_spheres.at(indexJoint)->setColor(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
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


void OsgQtWidget::updateSkeletonMeshCount()
{
	std::cout << "updateSkeletonMeshCount" << std::endl;
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
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.push_back(SkeletonMesh());

				}
			}
			// remove skeleton mesh from skeleton mesh pool when skeletonMeshPoolSize is bigger than skeletonPoolSize
			else if (skeletonMeshPoolSize > skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() > skeletonPoolTempCopy.size())
				{
					// remove skeletonMesh from skeletonMeshPool
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.pop_back();

				}
			}
		}
	}

	// unlock the trackerPool
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}