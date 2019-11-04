#include "MainWindow.h"
#include "ui_mainwindow.h"

// default constructor
MainWindow::MainWindow(InputManager* inputManager, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	// setup base class
	ui->setupUi(this);

	// assign reference to input manager
	m_refInputManager = inputManager;

	// register buttons of the window in input manager
	m_refInputManager->registerButton(0); // button: startAllTracker
	m_refInputManager->registerButton(1); // button: removeTracker

	m_updateThread = new std::thread(&MainWindow::update, this);
	m_updateThread->detach();

}

// default destructor
MainWindow::~MainWindow()
{

	// delete ui
	delete ui;

}

void MainWindow::update()
{

	m_refTrackerPool = m_refInputManager->getTrackerPool();

	while (true)
	{

		if (m_refInputManager->isTrackerDataAvailable())
		{

			ui->treeWidget_hirachy->clear();

			// tracker loop
			for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
			{

				QTreeWidgetItem* tracker = new QTreeWidgetItem();
				std::string trackerName = "tracker_" + itTrackerPool->first.first + "_" + std::to_string(itTrackerPool->first.second);
				tracker->setText(0, QString::fromStdString(trackerName));

				for (auto itSkeletonPool = itTrackerPool->second->begin(); itSkeletonPool != itTrackerPool->second->end(); itSkeletonPool++)
				{

					QTreeWidgetItem* skeleton = new QTreeWidgetItem();
					std::string skeletonName = "skeleton_" + std::to_string(itSkeletonPool->first);
					skeleton->setText(0, QString::fromStdString(skeletonName));

					tracker->addChild(skeleton);

				}

				ui->treeWidget_hirachy->addTopLevelItem(tracker);

			}
		}
	}	
}

// SLOT: close window / application
void MainWindow::on_actionExit_triggered()
{

	// close window
	this->close();

}

// SLOT: start all tracker
void MainWindow::startAllTracker()
{

	// toggle icon
	toggleIconStartButton();

	// set tracking
	m_isTracking = !m_isTracking;

	// set button start / stop pressed
	m_refInputManager->setButtonPressed(0, 1);

}

// SLOT: add new tracker
void MainWindow::addTracker()
{
	
	// create dialog for creating new tracker
	m_createTrackerWindow = new CreateTrackerWindow(m_refInputManager, ui->listView_tracker);
	
	// set modal and execute
	m_createTrackerWindow->setModal(true);
	m_createTrackerWindow->exec();


}

// SLOT: remove tracker button
void MainWindow::removeTracker()
{

	// id of tracker to remove
	int idToRemove = -1;

	// check if tracker exist in list
	if (ui->listView_tracker->selectionModel() != NULL)
	{

		// check if user selected tracker
		if (ui->listView_tracker->selectionModel()->selectedIndexes().count() == 0)
		{

			Console::logWarning("MainWindow::removeTracker(): No tracker in list selected!");
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

// remove tracker with id from list
void MainWindow::removeTrackerFromList(int id)
{

	// create new model and list
	QStringListModel* model = new QStringListModel(this);
	QStringList stringList;

	// create and get old model
	QAbstractItemModel* oldModel = ui->listView_tracker->model();

	// add old model data to list
	if (oldModel != nullptr)
	{
		for (size_t i = 0; i < oldModel->rowCount(); ++i)
		{

			if(i != id)
				stringList << oldModel->index(i, 0).data(Qt::DisplayRole).toString();
		}
	}

	// fill new model
	model->setStringList(stringList);
	// set model
	ui->listView_tracker->setModel(model);

}

// toogle icon of start / stop tracking button
void MainWindow::toggleIconStartButton()
{

	QIcon icon;

	if (!m_isTracking)
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-stop-30.png"), QSize(), QIcon::Normal, QIcon::Off);
	else
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32.png"), QSize(), QIcon::Normal, QIcon::Off);

	ui->btn_startTracker->setIcon(icon);

}