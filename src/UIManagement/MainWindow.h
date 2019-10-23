#pragma once

#include "ConfigDllExportUIManagement.h"
#include <QtWidgets/QMainWindow>
#include "MotionHubUtil/Console.h"
#include "InputManager.h"

namespace Ui {
	class MainWindow;
}

class UIManagement_DLL_import_export MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(/*InputManager* inputManager,*/ QWidget *parent = 0);
	~MainWindow();
	enum Buttons
	{
		startTracking
	};

private slots:
	void on_actionExit_triggered();
	void startAllTracker();

private:
	Ui::MainWindow *ui;
	//InputManager* m_refInputManager;

};