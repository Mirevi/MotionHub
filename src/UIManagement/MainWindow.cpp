#include "MainWindow.h"
//#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(InputManager* inputManager, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	ui->setupUi(this);

	m_refInputManager = inputManager;

	m_refInputManager->registerButton(0); //Button: startAllTracker
	m_refInputManager->registerButton(1); //Button: removeTracker

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionExit_triggered()
{
	this->close();
}

void MainWindow::startAllTracker()
{

	m_refInputManager->setButtonPressed(0, 1);

}

void MainWindow::addTracker()
{

	m_createTrackerWindow = new CreateTrackerWindow(m_refInputManager);
	
	m_createTrackerWindow->setModal(true);
	m_createTrackerWindow->exec();

}

void MainWindow::removeTracker()
{

	m_refInputManager->setButtonPressed(1, 1);

}