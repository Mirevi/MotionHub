#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	m_argc = argc;
	m_argv = argv;

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);
	
	m_inputManager = new InputManager();

	m_trackerManager = new TrackerManager();
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

	start();

	m_uiManager = new UIManager(m_argc, m_argv, m_inputManager);

}

void MotionHub::start()
{

	if(m_updateThread != nullptr)
	{

		m_updateThread = new std::thread(&MotionHub::update, this);
		m_updateThread->detach();

		Console::log("MotionHub::start(): Started update thread.");

	}
	else
		Console::logError("MotionHub::start(): Can not start update thread. Thread is already running!");

}

// motion hub loop
void MotionHub::update()
{

	while (true)
	{

		checkInput();

		if (m_isTracking)
		{

			// tracker loop
			for (auto itTracker = m_trackerManager->getPoolTracker()->begin(); itTracker != m_trackerManager->getPoolTracker()->end(); itTracker++)
			{

				if (itTracker->second->isDataAvailable())
				{

					//m_gestureManager->updateAllSkeletonPostures(&(itTracker->second->m_skeletonPool));
					m_networkManager->sendSkeletonPool(itTracker->second->getSkeletonPool());

				}
			}
		}
	}
}

void MotionHub::checkInput()
{

	if (m_inputManager->isButtonPressed(0))
	{

		if (!m_isTracking)
		{

			m_trackerManager->startTracker();
			m_isTracking = true;

			Console::log("MotionHub::checkInput(): Started tracking.");

		}
		else
		{

			m_trackerManager->stopTracker();
			m_isTracking = false;

			Console::log("MotionHub::checkInput(): Stopped tracking.");

		}
	}

	if (m_inputManager->isButtonPressed(1))
	{

		bool wasTracking = m_isTracking;

		// stop tracking loop if running
		if (m_isTracking)
		{

			stop();

		}

		m_trackerManager->removeTracker(m_inputManager->getSelectedTrackerIdInList());

		// start tracking loop if it was running
		if (wasTracking)
		{

			start();

		}
	}

	if (m_inputManager->isButtonPressed(2))
	{

		switch (m_inputManager->getSelectedTrackerIdInDropdown())
		{

			case 0:
				Console::log("MotionHub::checkInput(): Creating Azure Kinect tracker ...");
				m_trackerManager->createTracker(TrackerManager::AKT);
				break;

			default:
				Console::logError("MotionHub::checkInput(): Tracker type not yet implemented.");
				break;

		}
	}
}

void MotionHub::stop()
{

	m_isTracking = false;

	Console::log("MotionHub::start(): Stopped tracking loop.");

}