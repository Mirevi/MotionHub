#include "CreateTrackerWindow.h"
//#include "createtrackerwindow.h"
#include "ui_createtrackerwindow.h"

CreateTrackerWindow::CreateTrackerWindow(InputManager* inputManager, QWidget *parent) : QDialog(parent), ui(new Ui::CreateTrackerWindow)
{

	ui->setupUi(this);

	m_refInputManager = inputManager;

	m_refInputManager->registerButton(2);
}

CreateTrackerWindow::~CreateTrackerWindow()
{
	delete ui;
}

void CreateTrackerWindow::createTracker()
{

	m_refInputManager->setButtonPressed(2, 1);
	close();

}

void CreateTrackerWindow::switchTrackerType(int id)
{

	m_refInputManager->setCurrSelectedTrackerId(id);

}