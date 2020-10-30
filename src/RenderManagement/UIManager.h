#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtGui/qsurfaceformat.h>

#include "ConfigDllExportRenderManagement.h"

#include "MainWindow.h"
#include "CreateTrackerWindow.h"

#include <thread>

/*!
 * \class Manager for Qt UI window handling
 * 
 * \brief contains the Qt application and main window
 * 
 * \author Kester Evers and Eric Jansen
 * 
 */
class RenderManagement_DLL_import_export UIManager
{

public:
	/*!
	 * initializes QApplication and MainWindow
	 * 
	 * \param argc
	 * \param argv
	 * \param inputManager reference to the InputManager
	 */
	UIManager(int argc, char** argv, TrackerManager* trackerManager, NetworkManager* networkManager, ConfigManager* configManager);

	/*!
	 * getter for MainWindow pointer 
	 * \return pointer to the MainWindow Object
	 */
	MainWindow* getMainWindow();
	/*!
	 *  updates Qt UI
	 */
	void processInput();

private:
	/*!
	 * Main UI Qt application  
	 */
	QApplication* m_app;
	/*!
	 * main window containing all other windows 
	 */
	MainWindow* m_mainWindow;


};