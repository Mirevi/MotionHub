#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	// save arguments
	m_argc = argc;

	

	m_argv = argv;



	Console::log("MotionHub::MotionHub(): Starting Mirevi MotionHub");

	// create new config reader
	m_configManager = new ConfigManager();

	
	// create manager
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager(m_configManager);
	m_trackerManager = new TrackerManager(m_networkManager, m_configManager);

	m_uiManager		 = new UIManager(m_argc, m_argv, m_trackerManager, m_configManager);

	m_recordingThread = new std::thread(&MotionHub::updateRecorderThread, this);
	m_recordingThread->detach();

	RecordingSession::RECORD_PATH = m_configManager->getStringFromStartupConfig("recordPath");

	// start update loop
	update();

}

// motion hub main loop
void MotionHub::update()
{

	int framesElapsed = 0;

	// main loop
	while (m_uiManager->getMainWindow()->isVisible())
	{
		//Timer::reset();

		// process ui input
		m_uiManager->processInput();
		m_uiManager->getMainWindow()->updateConsole();

		// send skeleton pools to other managers
		if (m_trackerManager->isTracking())
		{

			std::vector<Tracker*> trackerPoolTempCopy = m_trackerManager->getPoolTracker();

			// iterates through all tracker located in the tracker manager tracker pool 
			for (auto itTracker = trackerPoolTempCopy.begin(); itTracker != trackerPoolTempCopy.end(); itTracker++)
			{

				// check if new skeleton data is available
				if ((*itTracker)->isDataAvailable())
				{

					//cache skeleton data of current tracker, so gesture- and network manager use current data
					(*itTracker)->cacheSkeletonData();

					// send skeleton pool reference to gesture manager in order to update all postures
					//m_gestureManager->updateAllSkeletonPostures(&((*itTracker)->getSkeletonPoolCache())									  );



					// update ui if skeleton was added or removed from pool
					if ((*itTracker)->hasSkeletonPoolChanged())
					{
						//Console::log("skeleton pool has changed!");

						//update UI
						m_uiManager->getMainWindow()->update();	


						m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshCount();

					}

					//Console::log("MotionHub::update(): number of skeletons = " + toString(m_trackerManager->getTrackerRefAt(0)->getProperties()->countDetectedSkeleton));

					m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshTransform();

					// reset bool and start new tracking cycle
					(*itTracker)->resetIsDataAvailable();

				}

			}


			//Recorder::instance().nextFrame();

			updateTimeline();
		}	//check if tracker is added or removed
		else if(m_trackerManager->hasTrackerPoolChanged())
		{

			//update UI
			m_uiManager->getMainWindow()->update();

			m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshPoolSize();

		}



		//Console::log("duration: " + toString(Timer::getDuration()));
	}



}

void MotionHub::updateTimeline()
{


	//get BVH/MMH current frame
	FrameData currFrameData = m_trackerManager->getRecCurrFrameData();

	if (currFrameData.currFrameIdx < 0)
	{
		return;
	}

	//and apply it to the timeline slider
	m_uiManager->getMainWindow()->setTimelineValue(currFrameData.totalTime, currFrameData.currFrameIdx, currFrameData.frameCount);



}

void MotionHub::updateRecorderThread()
{
	//wait frametime

	while (true)
	{

		//record skeletons
		if (Recorder::instance().isRecording())
		{
			//tell TrackerManager to record all active tracker
			m_trackerManager->writeSkeletonsToRecorder();

		}

		//wait frametime - elapsed time
		std::this_thread::sleep_for(std::chrono::milliseconds(FRAMETIME));
	}
}