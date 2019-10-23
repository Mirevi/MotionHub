#include "MainWindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(/*InputManager* inputManager, */QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	ui->setupUi(this);

	//m_refInputManager = inputManager;

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

	//m_refInputManager->setButtonPressed(MainWindow::startTracking);

}