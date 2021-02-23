#include "OsgQtWidget.h"

#include "TrackerManagement/TrackerManager.h"
#include <QVBoxLayout>


namespace osgQt
{
	class GraphicsWindowQt;
}

OsgQtWidget::OsgQtWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene, TrackerManager* trackerManager) :
	QWidget(), m_refTrackerManager(trackerManager)
{
	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	osg::Camera* camera = _viewer.getCamera();
	camera->setGraphicsContext(gw);
	camera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width) / static_cast<double>(traits->height), 0.1f, 1000.0f);

	_viewer.setSceneData(scene);
	_viewer.addEventHandler(new osgViewer::StatsHandler);
	_viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(gw->getGLWidget());
	setLayout(layout);

	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	_timer.start(10);
}


void OsgQtWidget::updateSkeletonMeshPoolSize()
{
	std::cout << "Test aus updateSkeletonMeshPoolSize" << std::endl;
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	m_refTrackerManager->getTrackerPoolLock()->lock();

	int trackerRefPoolSize = m_skeletonMeshPool.size();
	int trackerPoolSize = trackerTempCopy.size();

	// add tracker refferences to skeleton mesh pool
	if (trackerRefPoolSize < trackerPoolSize)
	{

		for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
		{

			if (m_skeletonMeshPool.find((*itTracker)->getProperties()->id) == m_skeletonMeshPool.end())
			{

				m_skeletonMeshPool.insert(std::make_pair((*itTracker)->getProperties()->id, std::vector<SkeletonMesh>()));

			}
		}
	}
	// remove tracker refferences from skeleton mesh pool
	else if (trackerRefPoolSize > trackerPoolSize)
	{

		while (m_skeletonMeshPool.size() > trackerTempCopy.size())
		{

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
	std::cout << "Test aus updateSkeletonMeshTransform" << std::endl;
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	m_refTrackerManager->getTrackerPoolLock()->lock();
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		if ((*itTracker)->isTracking())
		{

			int skeletonPoolSize = skeletonPoolTempCopy.size();
			int skeletonMeshPoolSize = m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size();

			// add skeleton mesh to skeleton mesh pool
			if (skeletonMeshPoolSize < skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() < skeletonPoolTempCopy.size())
				{

					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.push_back(SkeletonMesh());

				}
			}
			// remove skeleton mesh from skeleton mesh pool
			else if (skeletonMeshPoolSize > skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() > skeletonPoolTempCopy.size())
				{

					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.pop_back();

				}
			}
		}
	}
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}


void OsgQtWidget::updateSkeletonMeshCount()
{

	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	m_refTrackerManager->getTrackerPoolLock()->lock();
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		if ((*itTracker)->isTracking())
		{

			int skeletonPoolSize = skeletonPoolTempCopy.size();
			int skeletonMeshPoolSize = m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size();

			// add skeleton mesh to skeleton mesh pool
			if (skeletonMeshPoolSize < skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() < skeletonPoolTempCopy.size())
				{

					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.push_back(SkeletonMesh());

				}
			}
			// remove skeleton mesh from skeleton mesh pool
			else if (skeletonMeshPoolSize > skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() > skeletonPoolTempCopy.size())
				{

					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.pop_back();

				}
			}
		}
	}
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}