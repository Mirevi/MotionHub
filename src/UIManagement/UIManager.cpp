#include "UIManager.h"

UIManager::UIManager(int argc, char** argv, InputManager* inputManager)
{
	  
	m_refInputManager = inputManager;

	m_app = new QApplication(argc, argv);

	m_mainWindow = new MainWindow(m_refInputManager);
	m_mainWindow->show();

	m_app->exec();

}