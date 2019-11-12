#include "UIManager.h"

// default constructor
UIManager::UIManager(int argc, char** argv, TrackerManager* trackerManager)
{

	// create new qt ui application
	m_app = new QApplication(argc, argv);

	// create new main window and show window
	m_mainWindow = new MainWindow(trackerManager);
	m_mainWindow->show();

}

MainWindow* UIManager::getMainWindow()
{
	
	return m_mainWindow;

}

void UIManager::processInput()
{

	m_app->processEvents();

}