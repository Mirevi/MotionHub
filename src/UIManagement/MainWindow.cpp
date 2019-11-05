#include "MainWindow.h"
#include "ui_mainwindow.h"


// default constructor
MainWindow::MainWindow(TrackerManager* trackerManager, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	// setup base class
	ui->setupUi(this);

	// assign reference to tracker manager
	m_refTrackerManager = trackerManager;
	m_refTrackerPool = m_refTrackerManager->getPoolTracker();

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
	
	while (true)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(33));


		for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
		{
			if (itTrackerPool->second == nullptr)
			{

				break;

			}
			else if(itTrackerPool->second->hasSkeletonPoolChanged())
			{

				updateHirachy();
				break;

			}		
		}
		
	}	
}

void MainWindow::updateHirachy()
{

	if (ui->treeWidget_hirachy->topLevelItemCount() > 0)
	{

		ui->treeWidget_hirachy->clear();

	}

	// tracker loop
	for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
	{

		QTreeWidgetItem* tracker = new QTreeWidgetItem();
		std::string trackerName = "tracker_" + std::to_string(itTrackerPool->first.first) + "_" + std::to_string(itTrackerPool->first.second);
		tracker->setText(0, QString::fromStdString(trackerName));

		for (auto itSkeletonPool = itTrackerPool->second->getSkeletonPool()->begin(); itSkeletonPool != itTrackerPool->second->getSkeletonPool()->end(); itSkeletonPool++)
		{

			QTreeWidgetItem* skeleton = new QTreeWidgetItem();
			std::string skeletonName = "skeleton_" + std::to_string(itSkeletonPool->first);
			skeleton->setText(0, QString::fromStdString(skeletonName));

			tracker->addChild(skeleton);

		}

		ui->treeWidget_hirachy->addTopLevelItem(tracker);

		if (tracker->childCount() > 0)
			tracker->setExpanded(true);
		 
	}
}

// SLOT: close window / application
void MainWindow::on_actionExit_triggered()
{

	// close window
	this->close();

}

// SLOT: start all tracker
void MainWindow::slotStartTracker()
{

	// toggle icon
	toggleIconStartButton();

	// set tracking
	m_isTracking = !m_isTracking;

	// check if motion hub is tracking
	if (!m_refTrackerManager->isTracking())
	{
		m_refTrackerManager->startTracker(); // start tracking if false
	}
	else
	{
		m_refTrackerManager->stopTracker(); // stop tracking hub is true
	}
}

// SLOT: add new tracker
void MainWindow::slotAddTracker()
{
	
	// create dialog for creating new tracker
	m_createTrackerWindow = new CreateTrackerWindow(m_refTrackerManager, ui->listView_tracker);
	
	// set modal and execute
	m_createTrackerWindow->setModal(true);
	m_createTrackerWindow->exec();

	updateHirachy();

}

// SLOT: remove tracker button
void MainWindow::slotRemoveTracker()
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

		// save current tracking state
		bool wasTracking = m_refTrackerManager->isTracking();

		// check if motion hub is tracking
		if (m_refTrackerManager->isTracking())
			m_refTrackerManager->stopTracker(); // stop tracking if true

		// get selected tracker id in ui list
		// remove selected tracker from tracker pool
		m_refTrackerManager->removeTracker(idToRemove);

		// check if motion hub was tracking
		if (wasTracking)
			m_refTrackerManager->startTracker(); // start tracking if true

		// remove tracker from list
		removeTrackerFromList(idToRemove);

		updateHirachy();
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