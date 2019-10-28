#pragma once

#include "ConfigDllExportUIManagement.h"
#include <QtWidgets/QMainWindow>
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/InputManager.h"
#include "CreateTrackerWindow.h"
//#include <qstringlistmodel.h>
#include "QtCore/qstringlistmodel.h"
#include "QtCore/qstring.h"

namespace Ui {
	class MainWindow;
}

class UIManagement_DLL_import_export MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(InputManager* inputManager, QWidget *parent = 0);
	~MainWindow();
	CreateTrackerWindow* getCreateTrackerWindow();

private slots:
	void on_actionExit_triggered();
	void startAllTracker();
	void addTracker();
	void removeTracker();

private:
	Ui::MainWindow *ui;
	InputManager* m_refInputManager;
	CreateTrackerWindow* m_createTrackerWindow;
};