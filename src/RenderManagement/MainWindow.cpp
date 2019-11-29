#include "MainWindow.h"
#include "ui_mainwindow.h"

// default constructor
MainWindow::MainWindow(TrackerManager* trackerManager, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

	// setup base class
	ui->setupUi(this);

	render_ogl = new GlWidget(trackerManager);
	render_ogl->setObjectName(QStringLiteral("render_ogl"));
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
	render_ogl->setSizePolicy(sizePolicy2);

	ui->layout_center->addWidget(render_ogl);

	// assign reference to tracker manager
	m_refTrackerManager = trackerManager;
	m_refTrackerPool = m_refTrackerManager->getPoolTracker();

	// disable qt vector warning in console
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

// update ui content
void MainWindow::update()
{

	// update the hirachy
	updateHirachy();
	// update the inspector
	updateInspector();

}
  
#pragma region

void MainWindow::updateHirachy()
{
	// clear hirachy
	ui->treeWidget_hirachy->clear();
	// clear item pool
	m_hirachyItemPool.clear();
	 
	// loop throgh all tracker
	for (auto itTrackerPool = m_refTrackerPool->begin(); itTrackerPool != m_refTrackerPool->end(); itTrackerPool++)
	{

		// insert current tracker Item in map of top level items
		m_hirachyItemPool.insert({ new QTreeWidgetItem(), std::list<QTreeWidgetItem*>() });

		// get the trackers name and assign it to the display text
		std::string trackerName = itTrackerPool->second->getProperties()->name;
		m_hirachyItemPool.rbegin()->first->setText(0, QString::fromStdString(trackerName));

		// loop through all skeletons of the current tracker
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

	ui->treeWidget_hirachy->update();

}

void MainWindow::updateInspector()
{
	//check if selected tracker exists
	if (m_refTrackerManager->getTrackerRef(m_selectedTrackerInList) == nullptr)
	{
		//when this tracker doesn't exist, updating the inspector is not needed
		m_selectedTrackerInList = -1;
		return;

	}

	//get properties of selected tracker
	Tracker::Properties* trackerProperties = m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->getProperties();

	//set ID and name in inspector
	ui->tableWidget_inspector->item(0, 1)->setText(std::to_string(trackerProperties->id).c_str());
	ui->tableWidget_inspector->item(1, 1)->setText(trackerProperties->name.c_str());

	//check if tracker is tracking and set checkbox in inspector
	if (trackerProperties->isTracking)
	{
		
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Checked);

	}
	else
	{

		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Unchecked);

	}

	//check if tracker is enabled and set checkbox in inspector
	if (trackerProperties->isEnabled)
	{

		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);

	}
	else
	{

		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);

	}

	//set skeleton count in inspector
	ui->tableWidget_inspector->item(4, 1)->setText(std::to_string(trackerProperties->countDetectedSkeleton).c_str());

	//refresh the inspector to show new content
	ui->tableWidget_inspector->update();

}

void MainWindow::drawInspector()
{
	//check if selected tracker exists
	if (m_refTrackerManager->getTrackerRef(m_selectedTrackerInList) == nullptr)
	{
		//when this tracker doesn't exist, updating the inspector is not needed
		m_selectedTrackerInList = -1;
		return;

	}

	//reset the inspector
	clearInspector();

	//get properties of selected tracker
	Tracker::Properties* trackerProperties = m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->getProperties();

	//add ID row to inspector
	addRowToInspector("id", std::to_string(trackerProperties->id));
	ui->tableWidget_inspector->item(0, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(0, 1)->setFlags(Qt::NoItemFlags);

	//add name row to inspector
	addRowToInspector("name", trackerProperties->name);
	ui->tableWidget_inspector->item(1, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(1, 1)->setFlags(Qt::NoItemFlags);

	//add isTracking row to inspector
	addRowToInspector("isTracking", "");
	ui->tableWidget_inspector->item(2, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(2, 1)->setFlags(Qt::NoItemFlags);

	//insert checkbox into row, checked if tracker is tracking
	if (trackerProperties->isTracking)
	{
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Checked);
	}
	else
	{
		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Unchecked);
	}

	//add isEnabled row to inspector
	addRowToInspector("isEnabled", "");
	ui->tableWidget_inspector->item(3, 0)->setFlags(Qt::NoItemFlags);

	//insert checkbox into row, checked if tracker is enabled
	if (trackerProperties->isEnabled)
	{
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);
	}		
	else
	{
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);
	}
	
	//add skeleon count row to inspector
	addRowToInspector("countDetectedSkeleton", std::to_string(trackerProperties->countDetectedSkeleton));
	ui->tableWidget_inspector->item(4, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(4, 1)->setFlags(Qt::NoItemFlags);






	//add position offset rows to inspector
	addRowToInspector("position offset x", "");
	ui->tableWidget_inspector->item(5, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(5, 1, new QLineEdit(toQString(trackerProperties->positionOffset.x()), this));
	addRowToInspector("position offset y", "");
	ui->tableWidget_inspector->item(6, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(6, 1, new QLineEdit(toQString(trackerProperties->positionOffset.y()), this));
	addRowToInspector("position offset z", "");
	ui->tableWidget_inspector->item(7, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(7, 1, new QLineEdit(toQString(trackerProperties->positionOffset.z()), this));

	//add rotation offset rows to inspector
	addRowToInspector("rotation offset x", "");
	ui->tableWidget_inspector->item(8, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(8, 1, new QLineEdit(toQString(trackerProperties->rotationOffset.x()), this));
	addRowToInspector("rotation offset y", "");
	ui->tableWidget_inspector->item(9, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(9, 1, new QLineEdit(toQString(trackerProperties->rotationOffset.y()), this));
	addRowToInspector("rotation offset z", "");
	ui->tableWidget_inspector->item(10, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(10, 1, new QLineEdit(toQString(trackerProperties->rotationOffset.z()), this));

	//add scale offset rows to inspector
	addRowToInspector("scale offset x", "");
	ui->tableWidget_inspector->item(11, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(11, 1, new QLineEdit(toQString(trackerProperties->scaleOffset.x()), this));
	addRowToInspector("scale offset y", "");
	ui->tableWidget_inspector->item(12, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(12, 1, new QLineEdit(toQString(trackerProperties->scaleOffset.y()), this));
	addRowToInspector("scale offset z", "");
	ui->tableWidget_inspector->item(13, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->setCellWidget(13, 1, new QLineEdit(toQString(trackerProperties->scaleOffset.z()), this));




	// inspector has items
	m_isInspectorInit = true;

}

void MainWindow::clearInspector()
{

	// inspector has no items
	m_isInspectorInit = false;

	//delete content from inspector
	ui->tableWidget_inspector->clearContents();
	//delete all rows
	ui->tableWidget_inspector->setRowCount(0);

}

void MainWindow::addRowToInspector(std::string propertyName, std::string valueName)
{

	// get curr row count
	int currRow = ui->tableWidget_inspector->rowCount();
	// add new row to inspector
	ui->tableWidget_inspector->setRowCount(ui->tableWidget_inspector->rowCount() + 1);

	// create new property item and set text
	QTableWidgetItem* property = new QTableWidgetItem();
	property->setText(propertyName.c_str());

	// insert property item in current row
	ui->tableWidget_inspector->setItem(currRow, 0, property);

	// create new value item and set text
	QTableWidgetItem* value = new QTableWidgetItem();
	value->setText(valueName.c_str());

	// insert value item in current row
	ui->tableWidget_inspector->setItem(currRow, 1, value);

}

#pragma endregion Management

#pragma region

// SLOT: start all tracker
void MainWindow::slotToggleTracking()
{

	// set cursor to wait circle
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

	update();

	// reset cursor to default arrow
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

}

// SLOT: remove tracker button
void MainWindow::slotRemoveTracker()
{

	//set the curser to waiting
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::processEvents();

	// check if user selected item
	if (m_selectedTrackerInList > -1)
	{

		// remove tracker from tracker pool
		m_refTrackerManager->removeTracker(m_selectedTrackerInList);
		// remove tracker from tracker list
		ui->listWidget_tracker->takeItem(m_selectedTrackerInList);

		// set to no tracker selected
		m_selectedTrackerInList = -1;
		// clear the inspector
		clearInspector();

		ui->listWidget_tracker->clearSelection();

	}
	else
	{

		Console::logWarning("MainWindow::removeTracker(): No tracker in list selected!");

	}

	//set the curser to default
	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

}

// SLOT: get selected tracker index from tracker list
void MainWindow::slotSelectTracker(QModelIndex index)
{

	int previousSelectedTrackerInList = m_selectedTrackerInList;
	// get index of selected tracker
	m_selectedTrackerInList = ui->listWidget_tracker->currentRow();

	Console::log("MainWindow::slotSelectTracker(): Selected tracker with id = " + std::to_string(m_selectedTrackerInList));

	// update the inspector if current tracker was reselected
	if (previousSelectedTrackerInList == m_selectedTrackerInList)
		updateInspector();
	// if other tracker than before was selected - draw the ui with new content
	else
		drawInspector();
}

void MainWindow::slotInspectorItemChanged(QTableWidgetItem* item)
{
	
	// check if changed item is isEnabled checkbox of tracker
	if (m_isInspectorInit && item->row() == 3)
	{

		// set the curser to wait circle
		QApplication::setOverrideCursor(Qt::WaitCursor);
		QApplication::processEvents();

		// enabled or disable tracker based on check state
		if (item->checkState() == Qt::Checked)
			m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->enable();
		else if (item->checkState() == Qt::Unchecked)
			m_refTrackerManager->getTrackerRef(m_selectedTrackerInList)->disable();

		// update ui
		update();

		// reset cursor to default arrow
		QApplication::restoreOverrideCursor();
		QApplication::processEvents();

	}
}

// SLOT: close window / application
void MainWindow::on_actionExit_triggered()
{
	// close window
	this->close();

}

#pragma endregion Slots

#pragma region

// toogle icon of start / stop tracking button
void MainWindow::toggleTrackingButtons()
{

	QIcon icon;

	// if tracking is false set icon to start arrow and enbable add / remove tracker buttons
	if (!m_isTracking)
	{
		//load stop button
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-stop-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

		//disable add/remove buttons
		ui->btn_addTracker->setDisabled(true);
		ui->btn_removeTracker->setDisabled(true);

	}
	else
	{
		//load start button
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

		//enable add/remove buttons
		ui->btn_addTracker->setDisabled(false);
		ui->btn_removeTracker->setDisabled(false);

	}

	// set icon
	ui->btn_startTracker->setIcon(icon);

}

#pragma endregion Utils




QString MainWindow::toQString(float _float)
{

	//rounds to two decimals
	float temp = roundf(_float * 100) / 100;

	QString qstr = QString::fromUtf8(std::to_string(temp).c_str());

	//Console::log(qstr);

	return qstr;

}