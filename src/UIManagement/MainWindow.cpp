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

		if(!m_refTrackerManager->isTrackerPoolLocked())
		{ 
		
			for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
			{

				if (itTrackerPool->second->hasSkeletonPoolChanged())
				{

					updateHirachy();
					break;

				}
			}		
		}		
	}	
}

void MainWindow::updateHirachy()
{

	m_isHirachyLocked.store(true);

	// Console::log("MainWindow::updateHirachy(): Updating hirachy ...");

	ui->treeWidget_hirachy->clear();
	m_topLevelItemPool.clear();

	// tracker loop
	for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
	{

		m_topLevelItemPool.push_back(new QTreeWidgetItem());

		std::string trackerName = itTrackerPool->second->getName();
		m_topLevelItemPool.back()->setText(0, QString::fromStdString(trackerName));

		for (auto itSkeletonPool = itTrackerPool->second->getSkeletonPool()->begin(); itSkeletonPool != itTrackerPool->second->getSkeletonPool()->end(); itSkeletonPool++)
		{

			QTreeWidgetItem* skeleton = new QTreeWidgetItem();
			std::string skeletonName = "skeleton_" + std::to_string(itSkeletonPool->first);
			skeleton->setText(0, QString::fromStdString(skeletonName));

			m_topLevelItemPool.back()->addChild(skeleton);

		}

		ui->treeWidget_hirachy->addTopLevelItem(m_topLevelItemPool.back());

		//m_topLevelItemPool.back()->setExpanded(true);

		//tracker->setExpanded(true);

	}

	m_isHirachyLocked.store(false);

	//for (int indexItems = 0; indexItems < ui->treeWidget_hirachy->topLevelItemCount(); indexItems++)
	//{

	//	if(ui->treeWidget_hirachy->itemAt(indexItems, 0)->childCount() > 0)
	//		ui->treeWidget_hirachy->itemAt(indexItems, 0)->setExpanded(true);

	//}

	// ui->treeWidget_hirachy->expandToDepth(1);

	// Console::log("MainWindow::updateHirachy(): Updated hirachy.");

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
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::processEvents();

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

	QApplication::restoreOverrideCursor();
	QApplication::processEvents();
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