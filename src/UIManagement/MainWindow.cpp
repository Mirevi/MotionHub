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

	qRegisterMetaType<QVector<int>>();
	ui->tableWidget_inspector->setStyleSheet("::item:hover {background-color:rgba(0,0,0,0)}\n");

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

				if (itTrackerPool->second->hasSkeletonPoolChanged() && itTrackerPool->second->getProperties()->isEnabled)
				{

					updateHirachy();
					updatePropertiesInInspector();
					break;

				}
			}		
		}

		if (ui->tableWidget_inspector->rowCount() == 5 && m_wasTrackerInInspectorEnabled != ui->tableWidget_inspector->item(3, 1)->checkState())
		{

			m_wasTrackerInInspectorEnabled = ui->tableWidget_inspector->item(3, 1)->checkState();

			if (m_wasTrackerInInspectorEnabled == Qt::Checked)
			{

				m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->enable();

				updateHirachy();
				updatePropertiesInInspector();

			}
			else
			{

				m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->disable();

				updateHirachy();
				updatePropertiesInInspector();

			}
		}
	}	
}

void MainWindow::updateHirachy()
{

	m_isHirachyLocked.store(true);

	// Console::log("MainWindow::updateHirachy(): Updating hirachy ...");

	ui->treeWidget_hirachy->clear();
	m_hirachyItemPool.clear();

	// tracker loop
	for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
	{

		m_hirachyItemPool.insert({ new QTreeWidgetItem(), std::list<QTreeWidgetItem*>() });

		std::string trackerName = itTrackerPool->second->getProperties()->name;
		m_hirachyItemPool.rbegin()->first->setText(0, QString::fromStdString(trackerName));

		for (auto itSkeletonPool = itTrackerPool->second->getSkeletonPool()->begin(); itSkeletonPool != itTrackerPool->second->getSkeletonPool()->end(); itSkeletonPool++)
		{

			m_hirachyItemPool.rbegin()->second.push_back(new QTreeWidgetItem());
			std::string skeletonName = "skeleton_" + std::to_string(itSkeletonPool->first);
			m_hirachyItemPool.rbegin()->second.back()->setText(0, QString::fromStdString(skeletonName));

			m_hirachyItemPool.rbegin()->first->addChild(m_hirachyItemPool.rbegin()->second.back());

		}

		ui->treeWidget_hirachy->addTopLevelItem(m_hirachyItemPool.rbegin()->first);
			   
		m_hirachyItemPool.rbegin()->first->setExpanded(true);

		//tracker->setExpanded(true);

	}

	//updateInspector(m_selectedTrackerInList);

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
void MainWindow::slotToggleTracking()
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

	updatePropertiesInInspector();

}

// SLOT: add new tracker
void MainWindow::slotAddTracker()
{
	
	// create dialog for creating new tracker
	m_createTrackerWindow = new CreateTrackerWindow(m_refTrackerManager, ui->listWidget_tracker);
	
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
	m_selectedTrackerInList = -1;

	// check if tracker exist in list
	if (ui->listWidget_tracker->count() > 0)
	{

		// check if user selected tracker
		if (ui->listWidget_tracker->selectedItems().size() < 1)
		{

			Console::logWarning("MainWindow::removeTracker(): No tracker in list selected!");

			QApplication::restoreOverrideCursor();
			QApplication::processEvents();

			return;

		}
		else
		{

			m_selectedTrackerInList = ui->listWidget_tracker->currentIndex().data().toInt();

		}
	}
	else
	{

		Console::logError("MainWindow::removeTracker(): No tracker in list!");

		QApplication::restoreOverrideCursor();
		QApplication::processEvents();

		return;

	}

	// check if user selected item
	if (m_selectedTrackerInList > -1)
	{

		// save current tracking state
		bool wasTracking = m_refTrackerManager->isTracking();

		// check if motion hub is tracking
		if (m_refTrackerManager->isTracking())
			m_refTrackerManager->stopTracker(); // stop tracking if true

		// get selected tracker id in ui list
		// remove selected tracker from tracker pool
		m_refTrackerManager->removeTracker(m_selectedTrackerInList);

		ui->listWidget_tracker->takeItem(m_selectedTrackerInList);

		m_selectedTrackerInList = -1;

		// check if motion hub was tracking
		if (wasTracking)
			m_refTrackerManager->startTracker(); // start tracking if true

		updateHirachy();

	}

	removePropertiesFromInspector();

	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

}

// toogle icon of start / stop tracking button
void MainWindow::toggleIconStartButton()
{

	QIcon icon;

	if (!m_isTracking)
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-stop-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);
	else
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

	ui->btn_startTracker->setIcon(icon);

}

void MainWindow::slotSelectTracker(QModelIndex index)
{

	int previousSelectedTrackerInList = m_selectedTrackerInList;

	m_selectedTrackerInList = index.data().toInt();

	Console::log("MainWindow::slotSelectTracker(): Selected tracker with id = " + std::to_string(m_selectedTrackerInList));

	if (previousSelectedTrackerInList == m_selectedTrackerInList)
		updatePropertiesInInspector();
	else
		insertPropertiesIntoInspector();

}

void MainWindow::updatePropertiesInInspector()
{

	if (m_refTrackerManager->getTrackerRef(m_selectedTrackerInList) == nullptr)
	{

		m_selectedTrackerInList = -1;
		return;

	}

	while (m_isInspectorLocked.load())
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	m_isInspectorLocked.store(true);



	Tracker::Properties* trackerProperties = m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->getProperties();

	ui->tableWidget_inspector->item(0, 1)->setText(std::to_string(trackerProperties->id).c_str());
	ui->tableWidget_inspector->item(1, 1)->setText(trackerProperties->name.c_str());

	if (trackerProperties->isTracking)
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Checked);
	else
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Unchecked);


	if (trackerProperties->isEnabled)
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);
	else
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);

	ui->tableWidget_inspector->item(4, 1)->setText(std::to_string(trackerProperties->countDetectedSkeleton).c_str());

	ui->tableWidget_inspector->update();

	m_isInspectorLocked.store(false);

}

void MainWindow::insertPropertiesIntoInspector()
{

	if (m_refTrackerManager->getTrackerRef(m_selectedTrackerInList) == nullptr)
	{

		m_selectedTrackerInList = -1;
		return;

	}

	Tracker::Properties* trackerProperties = m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->getProperties();

	addRowToInspector("id", std::to_string(trackerProperties->id));
	ui->tableWidget_inspector->item(0, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(0, 1)->setFlags(Qt::NoItemFlags);

	addRowToInspector("name", trackerProperties->name);
	ui->tableWidget_inspector->item(1, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(1, 1)->setFlags(Qt::NoItemFlags);

	addRowToInspector("isTracking", "");
	ui->tableWidget_inspector->item(2, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(2, 1)->setFlags(Qt::NoItemFlags);

	if (trackerProperties->isTracking)
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Checked);
	else
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Unchecked);

	addRowToInspector("isEnabled", "");
	ui->tableWidget_inspector->item(3, 0)->setFlags(Qt::NoItemFlags);

	if (trackerProperties->isEnabled)
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);
	else
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);

	addRowToInspector("countDetectedSkeleton", std::to_string(trackerProperties->countDetectedSkeleton));
	ui->tableWidget_inspector->item(4, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(4, 1)->setFlags(Qt::NoItemFlags);

}

void MainWindow::removePropertiesFromInspector()
{

	ui->tableWidget_inspector->clearContents();
	ui->tableWidget_inspector->setRowCount(0);

}

void MainWindow::addRowToInspector(std::string propertyName, std::string valueName)
{

	int currRow = ui->tableWidget_inspector->rowCount();
	ui->tableWidget_inspector->setRowCount(ui->tableWidget_inspector->rowCount() + 1);

	QTableWidgetItem* property = new QTableWidgetItem();
	property->setText(propertyName.c_str());

	ui->tableWidget_inspector->setItem(currRow, 0, property);

	QTableWidgetItem* value = new QTableWidgetItem();
	value->setText(valueName.c_str());

	ui->tableWidget_inspector->setItem(currRow, 1, value);

}

std::string MainWindow::boolToString(bool b)
{

	return b ? "true" : "false";

}