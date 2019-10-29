#include "MainWindow.h"
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

	QIcon icon;

	if (!m_isTracking)
	{

		icon.addFile(QStringLiteral(":/ressources/icons/icons8-stop-30.png"), QSize(), QIcon::Normal, QIcon::Off);

	}
	else
	{

		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32.png"), QSize(), QIcon::Normal, QIcon::Off);
	
	}

	m_isTracking = !m_isTracking;

	ui->btn_startTracker->setIcon(icon);

	m_refInputManager->setButtonPressed(0, 1);

}

void MainWindow::addTracker()
{
	

	m_createTrackerWindow = new CreateTrackerWindow(m_refInputManager, ui->listView_tracker);
	
	m_createTrackerWindow->setModal(true);
	m_createTrackerWindow->exec();

}

void MainWindow::removeTracker()
{

	int idToRemove = -1;

	if (ui->listView_tracker->selectionModel() != NULL)
	{

		if (ui->listView_tracker->selectionModel()->selectedIndexes().count() == 0)
		{

			Console::logError("MainWindow::removeTracker(): No tracker in list selected!");
			return;

		}
		else
		{

			// get selected item id
			for each (QModelIndex index in ui->listView_tracker->selectionModel()->selectedIndexes())
			{

				idToRemove = index.data().toInt();

			}
		}
	}
	else
	{

		Console::logError("MainWindow::removeTracker(): No tracker in list!");
		return;

	}

	// check if user selected item
	if (idToRemove > -1)
	{

		// save id in input manager
		m_refInputManager->setSelectedTrackerIdInList(idToRemove);

		// set button press for input loop
		m_refInputManager->setButtonPressed(1, 1);

		// remove tracker from list
		removeTrackerFromList(idToRemove);

	}
}

void MainWindow::removeTrackerFromList(int id)
{

	QStringListModel* model = new QStringListModel(this);
	QStringList stringList;

	QAbstractItemModel* oldModel = ui->listView_tracker->model();

	if (oldModel != nullptr)
	{
		for (size_t i = 0; i < oldModel->rowCount(); ++i)
		{

			if(i != id)
				stringList << oldModel->index(i, 0).data(Qt::DisplayRole).toString();
		}
	}

	model->setStringList(stringList);
	ui->listView_tracker->setModel(model);

}