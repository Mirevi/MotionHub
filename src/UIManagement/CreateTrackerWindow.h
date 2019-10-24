#pragma once

#include "ConfigDllExportUIManagement.h"
#include <QtWidgets/QDialog>
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/InputManager.h"

namespace Ui {
	class CreateTrackerWindow;
}

class UIManagement_DLL_import_export CreateTrackerWindow : public QDialog
{
	Q_OBJECT
public:
	explicit CreateTrackerWindow(InputManager* inputManager, QWidget *parent = 0);
	~CreateTrackerWindow();

private slots:
	void createTracker();
	void switchTrackerType(int id);

private:
	Ui::CreateTrackerWindow *ui;
	InputManager* m_refInputManager;

};