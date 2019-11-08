#include "MainWindow.h"
#include "ui_mainwindow.h"

// default constructor
MainWindow::MainWindow(TrackerManager* trackerManager, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	// setup base class
	ui->setupUi(this);

	m_isHirachyLocked.store(false);
	m_isInspectorLocked.store(false);

	// assign reference to tracker manager
	m_refTrackerManager = trackerManager;
	m_refTrackerPool = m_refTrackerManager->getPoolTracker();

	qRegisterMetaType<QVector<int>>();
	// disable highlighting of cells when hovering over them
	ui->tableWidget_inspector->setStyleSheet("::item:hover {background-color:rgba(0,0,0,0)}\n");

}

// default destructor
MainWindow::~MainWindow()
{

	// delete ui
	delete ui;

}

void MainWindow::update()
{

	// update hirachy and inspector if skeleton was added or removed
	if(!m_refTrackerManager->isTrackerPoolLocked())
	{ 
		
		for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
		{

			if (/*itTrackerPool->second->isDataAvailable() && */itTrackerPool->second->hasSkeletonPoolChanged() && itTrackerPool->second->getProperties()->isEnabled)
			{

				updateHirachy();
				updateInspector();
				return;

			}
		}		
	}
}
  
void MainWindow::updateHirachy()
{

	//while (m_isInspectorLocked.load())
	//{

	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//}

	//m_isHirachyLocked.store(true);

	// Console::log("MainWindow::updateHirachy(): Updating hirachy ...");

	ui->treeWidget_hirachy->clear();
	m_hirachyItemPool.clear();

	// loop throgh all tracker
	for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
	{
		//insert current tracker Item in map of top level items
		m_hirachyItemPool.insert({ new QTreeWidgetItem(), std::list<QTreeWidgetItem*>() });

		//get the trackers name and assign it to the display text
		std::string trackerName = itTrackerPool->second->getProperties()->name;
		m_hirachyItemPool.rbegin()->first->setText(0, QString::fromStdString(trackerName));

		//loop through all skeletons of the current tracker
		for (auto itSkeletonPool = itTrackerPool->second->getSkeletonPool()->begin(); itSkeletonPool != itTrackerPool->second->getSkeletonPool()->end(); itSkeletonPool++)
		{
			//insert current skeleton Item in list of child items
			m_hirachyItemPool.rbegin()->second.push_back(new QTreeWidgetItem());

			//get the skeletons id and assign it to the display text
			std::string skeletonName = "skeleton_" + std::to_string(itSkeletonPool->first);
			m_hirachyItemPool.rbegin()->second.back()->setText(0, QString::fromStdString(skeletonName));

			//add child item to top level item
			m_hirachyItemPool.rbegin()->first->addChild(m_hirachyItemPool.rbegin()->second.back());

		}
		
		//add top level item to hirachy
		ui->treeWidget_hirachy->addTopLevelItem(m_hirachyItemPool.rbegin()->first);
		

		//expand all items in hirachy
		m_hirachyItemPool.rbegin()->first->setExpanded(true);

	}

	//m_isHirachyLocked.store(false);

}

// SLOT: close window / application
void MainWindow::on_actionExit_triggered()
{

	// close window
	this->close();

}

// toogle icon of start / stop tracking button
void MainWindow::toggleTrackingButtons()
{

	QIcon icon;

	if (!m_isTracking)
	{

		icon.addFile(QStringLiteral(":/ressources/icons/icons8-stop-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

		ui->btn_addTracker->setDisabled(true);
		ui->btn_removeTracker->setDisabled(true);

	}
	else
	{

		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

		ui->btn_addTracker->setDisabled(false);
		ui->btn_removeTracker->setDisabled(false);

	}

	ui->btn_startTracker->setIcon(icon);

}

void MainWindow::updateInspector()
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

void MainWindow::drawInspector()
{

	if (m_refTrackerManager->getTrackerRef(m_selectedTrackerInList) == nullptr)
	{

		m_selectedTrackerInList = -1;
		return;

	}

	clearInspector();

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

	m_isInspectorInit = true;

}

void MainWindow::clearInspector()
{

	m_isInspectorInit = false;

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



#pragma region Region

// SLOT: start all tracker
void MainWindow::slotToggleTracking()
{

	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::processEvents();

	// toggle buttons
	toggleTrackingButtons();

	// set tracking
	m_isTracking = !m_isTracking;

	// check if motion hub is tracking
	if (!m_refTrackerManager->isTracking())
		m_refTrackerManager->startTracker(); // start tracking if false
	else
		m_refTrackerManager->stopTracker(); // stop tracking if true

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	updateHirachy();
	updateInspector();

	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

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

	// check if user selected item
	if (m_selectedTrackerInList > -1)
	{

		m_refTrackerManager->removeTracker(m_selectedTrackerInList);

		ui->listWidget_tracker->takeItem(m_selectedTrackerInList);

		updateHirachy();
		clearInspector();

		m_selectedTrackerInList = -1;

	}
	else
	{

		Console::logWarning("MainWindow::removeTracker(): No tracker in list selected!");

	}

	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

}

// SLOT: get selected tracker index from tracker list
void MainWindow::slotSelectTracker(QModelIndex index)
{

	int previousSelectedTrackerInList = m_selectedTrackerInList;
	m_selectedTrackerInList = index.data().toInt();

	Console::log("MainWindow::slotSelectTracker(): Selected tracker with id = " + std::to_string(m_selectedTrackerInList));

	if (previousSelectedTrackerInList == m_selectedTrackerInList)
		updateInspector();
	else
		drawInspector();

}

void MainWindow::slotInspectorItemChanged(QTableWidgetItem* item)
{
	
	if (m_isInspectorInit && item->row() == 3)
	{

		QApplication::setOverrideCursor(Qt::WaitCursor);
		QApplication::processEvents();

		if (item->checkState() == Qt::Checked)
			m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->enable();
		else if (item->checkState() == Qt::Unchecked)
			m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->disable();

		updateHirachy();
		updateInspector();

		QApplication::restoreOverrideCursor();
		QApplication::processEvents();

	}
}

#pragma endregion Slots

#pragma region Region

std::string MainWindow::boolToString(bool b)
{

	return b ? "true" : "false";

}

#pragma endregion Utils