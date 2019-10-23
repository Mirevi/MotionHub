#include "UIManager.h"

UIManager::UIManager(int argc, char** argv/*, InputManager* inputManager*/)
{
	   
	m_threadApplication = new std::thread(&UIManager::init, this, argc, argv/*, inputManager*/);
	m_threadApplication->detach();

}

void UIManager::init(int argc, char** argv/*, InputManager* inputManager*/)
{

	m_app = new QApplication(argc, argv);

	m_mainWindow = new MainWindow(/*inputManager*/);
	m_mainWindow->show();

	m_app->exec();

}

MainWindow* UIManager::getMainWindow()
{

	return m_mainWindow;

}