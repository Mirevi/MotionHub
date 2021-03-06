#include "UIManager.h"

// default constructor
UIManager::UIManager(int argc, char** argv, TrackerManager* trackerManager, ConfigManager* configManager)
{

	// create new qt ui application
	m_app = new QApplication(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);
	

	// create new main window and show window
	m_mainWindow = new MainWindow(trackerManager, configManager);
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