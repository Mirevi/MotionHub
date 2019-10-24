#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>

#include "ConfigDllExportUIManagement.h"

#include "MainWindow.h"
#include "CreateTrackerWindow.h"
#include "MotionHubUtil/InputManager.h"

#include <thread>

class UIManagement_DLL_import_export UIManager
{

public:
	UIManager(int argc, char** argv, InputManager* inputManager);

private:
	QApplication* m_app;
	MainWindow* m_mainWindow;
	CreateTrackerWindow* m_createTrackerWindow;
	InputManager* m_refInputManager;


};