#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>

#include "ConfigDllExportUIManagement.h"

#include "MainWindow.h"
#include "InputManager.h"

#include <thread>

class UIManagement_DLL_import_export UIManager
{

public:
	UIManager(int argc, char** argv/*, InputManager* inputManager*/);
	MainWindow* getMainWindow();

private:
	void init(int argc, char** argv/*, InputManager* inputManager*/);
	QApplication* m_app;
	MainWindow* m_mainWindow;
	std::thread* m_threadApplication; 

};