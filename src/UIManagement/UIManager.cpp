#include "UIManager.h"

// default constructor
UIManager::UIManager(int argc, char** argv, InputManager* inputManager, TrackerManager* trackerManager)
{
	
	// get reference to input manger
	m_refInputManager = inputManager;

	// get reference to tracker manager
	m_refTrackerManager = trackerManager;

	// create new qt ui application
	m_app = new QApplication(argc, argv);

	// create new main window and show window
	m_mainWindow = new MainWindow(m_refInputManager, m_refTrackerManager);
	m_mainWindow->show();

	// MAIN THREAD CONTINUES HERE:

	// execute application
	m_app->exec();

}