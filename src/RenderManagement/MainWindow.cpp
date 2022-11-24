#include "MainWindow.h"
#include "ui_mainwindow.h"


// default constructor
MainWindow::MainWindow(TrackerManager* trackerManager, ConfigManager* configManager, QWidget* parent)
 : QMainWindow(parent), ui(new Ui::MainWindow)
{
	// assign reference to tracker manager
	m_refTrackerManager = trackerManager;
	m_configManager = configManager;

	m_timelineLableState = percentage;

	osg::Vec4 test(1.0, 1.0, 1.0, 1.0);
	// setup base class
	ui->setupUi(this);

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowDecoration = false;
	traits->x = 0;
	traits->y = 0;
	traits->width = 640;
	traits->height = 480;
	traits->doubleBuffer = true;
	traits->samples = 4; //Anti Alaising

	osgQt::GraphicsWindowQt* gw = new osgQt::GraphicsWindowQt(traits.get());
	m_osgQtWidget = new OsgQtWidget(gw, m_refTrackerManager, m_configManager);
	m_osgQtWidget->setObjectName(QStringLiteral("OsgRenderer"));
	QSizePolicy sizePolicyOsgQtWidget(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_osgQtWidget->setSizePolicy(sizePolicyOsgQtWidget);
	ui->gridLayout_main->addWidget(m_osgQtWidget);

	m_isTimelinePlaying = true;
	m_isLooping = true;

	connect(m_osgQtWidget, SIGNAL(osgWidgetPressed(osg::Vec2 position2d)),	this, SLOT(slotOsgWidgetPressed(osg::Vec2 position2d))	);
	connect(m_osgQtWidget, SIGNAL(osgWidgetReleased(osg::Vec2 position2d)), this, SLOT(slotOsgWidgetReleased(osg::Vec2 position2d))	);
	connect(m_osgQtWidget, SIGNAL(osgWidgetMoved(osg::Vec2 position2d)),	this, SLOT(slotOsgWidgetMoved(osg::Vec2 position2d)	)	);



	m_osgQtWidget->show();

	//add timeline-Widget to MainWindow
	m_mmhTimeline = new mmhTimeline();
	ui->verticalLayout_timeline->addWidget(m_mmhTimeline);

	//connect all of the Widgets Signals to the slots here
	connect(m_mmhTimeline, SIGNAL(timelinePressed(float)),	this, SLOT(slotTimelinePressed(float)		));
	connect(m_mmhTimeline, SIGNAL(timelineReleased(float)), this, SLOT(slotTimelineReleased(float)		));
	connect(m_mmhTimeline, SIGNAL(timelineMoved(float)),	this, SLOT(slotTimelineValueChanged(float)	));
	
	//sets the Timeline visible
	m_mmhTimeline->show();

	//hides progressbar
	ui->progressBar_save->hide();



	//maximize the MainWindow
	this->setWindowState(Qt::WindowMaximized);


	// disable qt vector warning in console
	qRegisterMetaType<QVector<int>>();
	// disable highlighting of cells when hovering over them
	ui->tableWidget_inspector->setStyleSheet("::item:hover {background-color:rgba(0,0,0,0)}\n");
	ui->tableWidget_inspector->setRowCount(14);
	ui->tableWidget_inspector->setColumnCount(2);
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 14; j++)
		{
			ui->tableWidget_inspector->setItem(j, i, new QTableWidgetItem());
		}
	}

	ui->tableWidget_console->setColumnWidth(1, 70);

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

	//update Console
	updateConsole();

}

#pragma region

void MainWindow::updateHirachy()
{
	//m_osgQtWidget->update();
	// clear hirachy
	ui->treeWidget_hirachy->clear();
	// clear item pool
	m_hirachyItemPool.clear();

	std::vector<Tracker*> trackerPoolTempCopy = m_refTrackerManager->getPoolTracker();


	// loop over all tracker
	for (auto itTrackerPool = trackerPoolTempCopy.begin(); itTrackerPool != trackerPoolTempCopy.end(); itTrackerPool++)
	{

		//MAYBE LOCK TRACKERPOOL HERE

		// insert current tracker Item in map of top level items
		m_hirachyItemPool.insert({ new QTreeWidgetItem(), std::list<QTreeWidgetItem*>() });

		// get the trackers name and assign it to the display text
		std::string trackerName = (*itTrackerPool)->getProperties()->name;
		m_hirachyItemPool.rbegin()->first->setText(0, QString::fromStdString(trackerName));

		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTrackerPool)->getSkeletonPoolCache();


		// loop through all skeletons of the current tracker
		for (auto itSkeletonPool = skeletonPoolTempCopy.begin(); itSkeletonPool != skeletonPoolTempCopy.end(); itSkeletonPool++)
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
	if (m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList) == nullptr)
	{
		//when this tracker doesn't exist, updating the inspector is not needed				
		m_selectedTrackerInList = -1;

		return;

	}

	//get selected tracker and its properties
	Tracker* tracker = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList);
	Tracker::Properties* trackerProperties = tracker->getProperties();
	//Console::logWarning("MainWindow::updateInspector()");

	//set ID and name in inspector
	ui->tableWidget_inspector->item(0, 1)->setText(std::to_string(trackerProperties->id).c_str());
	//ui->tableWidget_inspector->item(0, 1)->setText(std::to_string(1).c_str());
	ui->tableWidget_inspector->item(1, 1)->setText(trackerProperties->name.c_str());

	//check if tracker is tracking and set checkbox in inspector
	if (tracker->isTracking())
	{

		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Checked);

	}
	else
	{

		ui->tableWidget_inspector->item(2, 1)->setCheckState(Qt::Unchecked);

	}

	//check if tracker is enabled and set checkbox in inspector
	if (tracker->isEnabled())
	{

		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);

	}
	else
	{

		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);

	}

	//set skeleton count in inspector
	ui->tableWidget_inspector->item(4, 1)->setText(std::to_string(tracker->getNumDetectedSkeletons()).c_str());

	//refresh the inspector to show new content
	ui->tableWidget_inspector->update();


}

void MainWindow::updateConsole()
{

	std::vector<Console::Message> newLogs = Console::getMessages();

	for (int i = 0; i < newLogs.size(); i++)
	{
		//get time, type and message
		QTableWidgetItem* currItemTime		= new QTableWidgetItem(QString::fromStdString(newLogs[i].time));
		QTableWidgetItem* currItemType		= new QTableWidgetItem(QString::fromStdString(newLogs[i].type));
		QTableWidgetItem* currItemMessage	= new QTableWidgetItem(QString::fromStdString(newLogs[i].message));
		
		//align the columns left
		currItemTime->setTextAlignment(Qt::AlignLeft); //aligning is inverted, this is actually left aligned
		currItemType->setTextAlignment(Qt::AlignLeft);
		currItemMessage->setTextAlignment(Qt::AlignLeft);



		//insert new row
		ui->tableWidget_console->insertRow(ui->tableWidget_console->rowCount());
	
		//insert time
		ui->tableWidget_console->setItem(ui->tableWidget_console->rowCount() - 1, 0, currItemTime);
		//insert type
		ui->tableWidget_console->setItem(ui->tableWidget_console->rowCount() - 1, 1, currItemType);
		//insert message
		ui->tableWidget_console->setItem(ui->tableWidget_console->rowCount() - 1, 2, currItemMessage);

		//std::cout << "MainWindow::updateConsole(): log: " << currItem->text().toLocal8Bit().constData() << std::endl;

		ui->tableWidget_console->scrollToBottom();

	}

}

void MainWindow::drawInspector()
{
	//check if selected tracker exists
	if (m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList) == nullptr)
	{

		//when this tracker doesn't exist, updating the inspector is not needed
		m_selectedTrackerInList = -1;
		return;

	}

	//reset the inspector
	clearInspector();

	//get selected tracker and its properties
	Tracker* tracker = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList);
	Tracker::Properties* trackerProperties = tracker->getProperties();

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
	if (tracker->isTracking())
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
	if (tracker->isEnabled())
	{
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Checked);
	}
	else
	{
		ui->tableWidget_inspector->item(3, 1)->setCheckState(Qt::Unchecked);
	}

	//add skeleon count row to inspector
	addRowToInspector("countDetectedSkeleton", std::to_string(tracker->getNumDetectedSkeletons()));
	ui->tableWidget_inspector->item(4, 0)->setFlags(Qt::NoItemFlags);
	ui->tableWidget_inspector->item(4, 1)->setFlags(Qt::NoItemFlags);

	m_inputFieldPool.clear();

	//add input lines for the offset matrix to the Inspector

	//create new QLineEdit Object and assingn text to it
	m_inputFieldPool.insert({ "posX", new QLineEdit(toQString(trackerProperties->positionOffset.x()), this) });
	m_inputFieldPool.at("posX")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	//connect its textEdited() signal with the correct slot
	connect(m_inputFieldPool.at("posX"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputPosX(QString)));
	//add new row to the table widget
	addRowToInspector("position offset x", "");
	//add QLineEdit object to cell
	ui->tableWidget_inspector->setCellWidget(5, 1, m_inputFieldPool.at("posX"));

	//repeat for y and z
	m_inputFieldPool.insert({ "posY", new QLineEdit(toQString(trackerProperties->positionOffset.y()), this) });
	m_inputFieldPool.at("posY")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("posY"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputPosY(QString)));
	addRowToInspector("position offset y", "");
	ui->tableWidget_inspector->setCellWidget(6, 1, m_inputFieldPool.at("posY"));

	m_inputFieldPool.insert({ "posZ", new QLineEdit(toQString(trackerProperties->positionOffset.z()), this) });
	m_inputFieldPool.at("posZ")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("posZ"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputPosZ(QString)));
	addRowToInspector("position offset z", "");
	ui->tableWidget_inspector->setCellWidget(7, 1, m_inputFieldPool.at("posZ"));


	//repeat for rotation
	m_inputFieldPool.insert({ "rotX", new QLineEdit(toQString(trackerProperties->rotationOffset.x()), this) });
	m_inputFieldPool.at("rotX")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("rotX"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputRotX(QString)));
	addRowToInspector("rotation offset x", "");
	ui->tableWidget_inspector->setCellWidget(8, 1, m_inputFieldPool.at("rotX"));

	m_inputFieldPool.insert({ "rotY", new QLineEdit(toQString(trackerProperties->rotationOffset.y()), this) });
	m_inputFieldPool.at("rotY")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("rotY"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputRotY(QString)));
	addRowToInspector("rotation offset y", "");
	ui->tableWidget_inspector->setCellWidget(9, 1, m_inputFieldPool.at("rotY"));

	m_inputFieldPool.insert({ "rotZ", new QLineEdit(toQString(trackerProperties->rotationOffset.z()), this) });
	m_inputFieldPool.at("rotZ")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("rotZ"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputRotZ(QString)));
	addRowToInspector("rotation offset z", "");
	ui->tableWidget_inspector->setCellWidget(10, 1, m_inputFieldPool.at("rotZ"));


	//repeat for scale
	m_inputFieldPool.insert({ "scaleX", new QLineEdit(toQString(trackerProperties->scaleOffset.x()), this) });
	m_inputFieldPool.at("scaleX")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("scaleX"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputScaleX(QString)));
	addRowToInspector("scale offset x", "");
	ui->tableWidget_inspector->setCellWidget(11, 1, m_inputFieldPool.at("scaleX"));

	m_inputFieldPool.insert({ "scaleY", new QLineEdit(toQString(trackerProperties->scaleOffset.y()), this) });
	m_inputFieldPool.at("scaleY")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("scaleY"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputScaleY(QString)));
	addRowToInspector("scale offset y", "");
	ui->tableWidget_inspector->setCellWidget(12, 1, m_inputFieldPool.at("scaleY"));

	m_inputFieldPool.insert({ "scaleZ", new QLineEdit(toQString(trackerProperties->scaleOffset.z()), this) });
	m_inputFieldPool.at("scaleZ")->setValidator(new QDoubleValidator(-1000.0f, 1000.0f, 4, this));
	connect(m_inputFieldPool.at("scaleZ"), SIGNAL(textEdited(const QString&)), this, SLOT(slotInspectorInputScaleZ(QString)));
	addRowToInspector("scale offset z", "");
	ui->tableWidget_inspector->setCellWidget(13, 1, m_inputFieldPool.at("scaleZ"));

	//add additional properties
	int numAdditionalProperties = 0;
	for (const auto& kv : trackerProperties->additionalProperties)
	{
		if (kv.second->isType<bool>()) {
			//Console::log("Property type: BOOL");
			QCheckBox* checkBox = new QCheckBox(this);
			if (((Tracker::Property<bool>*) kv.second)->value)
			{
				checkBox->setChecked(true);
			}
			connect(checkBox, &QCheckBox::clicked, [=](bool state) { tracker->setPropertyValue<bool>(kv.first, state); Console::log(std::to_string(((Tracker::Property<bool>*) kv.second)->value)); });
			addRowToInspector(kv.second->name, "");
			ui->tableWidget_inspector->setCellWidget(14 + numAdditionalProperties, 1, checkBox);
		}
		else if (kv.second->isType<int>()) {
			//Console::log("Property type: INT");
			m_inputFieldPool.insert({ kv.first, new QLineEdit(toQString(((Tracker::Property<int>*) kv.second)->value), this) });
			m_inputFieldPool.at(kv.first)->setValidator(new QIntValidator(this));
			connect(m_inputFieldPool.at(kv.first), &QLineEdit::textEdited, [=](const QString& text) { tracker->setPropertyValue<int>(kv.first, text.toInt()); });
			addRowToInspector(kv.second->name, "");
			ui->tableWidget_inspector->setCellWidget(14 + numAdditionalProperties, 1, m_inputFieldPool.at(kv.first));
		}
		else if (kv.second->isType<float>()) {
			//Console::log("Property type: FLOAT");
			m_inputFieldPool.insert({ kv.first, new QLineEdit(toQString(((Tracker::Property<float>*) kv.second)->value), this) });
			m_inputFieldPool.at(kv.first)->setValidator(new QDoubleValidator(this));
			connect(m_inputFieldPool.at(kv.first), &QLineEdit::textEdited, [=](const QString& text) { tracker->setPropertyValue<float>(kv.first, text.toFloat()); });
			addRowToInspector(kv.second->name, "");
			ui->tableWidget_inspector->setCellWidget(14 + numAdditionalProperties, 1, m_inputFieldPool.at(kv.first));
		}
		else if (kv.second->isType<std::string>()) {
			//Console::log("Property type: STRING");
			m_inputFieldPool.insert({ kv.first, new QLineEdit(((Tracker::Property<std::string>*) kv.second)->value.c_str(), this) });
			connect(m_inputFieldPool.at(kv.first), &QLineEdit::textEdited, [=](const QString& text) { tracker->setPropertyValue<std::string>(kv.first, text.toStdString()); });
			addRowToInspector(kv.second->name, "");
			ui->tableWidget_inspector->setCellWidget(14 + numAdditionalProperties, 1, m_inputFieldPool.at(kv.first));
		}
		else continue;
		numAdditionalProperties++;
		// //Add reset button
		// QPushButton* modPushButton = new QPushButton("Modify Tracker");
		// //connect button wit reset slot
		// connect(modPushButton, SIGNAL(pressed()), this, SLOT(slotModifyTrackerRotations()));
		// addRowToInspector("Rotations", "");
		// ui->tableWidget_inspector->setCellWidget(15 + numAdditionalProperties, 1, modPushButton);
	}
	//disable item selection on all table cells
	for (int i = 5; i < 14 + numAdditionalProperties; i++) ui->tableWidget_inspector->item(i, 0)->setFlags(Qt::NoItemFlags);


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
	//Console::log("Nicht sicher");
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
	//QApplication::processEvents();

	// toggle buttons
	toggleTrackingButtons();

	// set tracking
	m_isTracking = !m_isTracking;

	// check if motion hub is tracking
	if (!m_refTrackerManager->isTracking())
	{

		m_refTrackerManager->startTracker(); // start tracking if false
		//m_timelineActive = true;
		ui->btn_Record->setEnabled(true);

	}
	else
	{
		if (Recorder::instance().isRecording())
		{
			Record();
		}

		m_refTrackerManager->stopTracker(); // stop tracking if true
		//m_timelineActive = false;
		ui->btn_Record->setEnabled(false);

	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	update();

	// reset cursor to default arrow
	QApplication::restoreOverrideCursor();
	//QApplication::processEvents();

}

// SLOT: add new tracker
void MainWindow::slotAddTracker()
{

	// create dialog for creating new tracker
	m_createTrackerWindow = new CreateTrackerWindow(m_refTrackerManager, ui->treeWidget_tracker);
	
	// set modal and execute
	m_createTrackerWindow->setModal(true);
	m_createTrackerWindow->exec();

}

void MainWindow::slotAddGroup()
{

	//change curser to wait (Hourglass)
	QApplication::setOverrideCursor(Qt::WaitCursor);

	int id = -1;

	id = m_refTrackerManager->createTracker(TrackerManager::group);

	addTrackerToList(id);

	//set the curser to default
	QApplication::restoreOverrideCursor();

}

// SLOT: remove tracker button
void MainWindow::slotRemoveTracker()
{

	//set the curser to waiting
	QApplication::setOverrideCursor(Qt::WaitCursor);

	// check if user selected item
	if (m_selectedTrackerInList > -1)
	{

		//// remove tracker from tracker pool
		m_refTrackerManager->removeTrackerAt(m_selectedTrackerInList);
		// remove tracker from tracker list
		delete ui->treeWidget_tracker->takeTopLevelItem(m_selectedTrackerInList);

		// set to no tracker selected
		m_selectedTrackerInList = -1;
		// clear the inspector
		clearInspector();

		ui->treeWidget_tracker->clearSelection();

	}
	else
	{

		Console::logWarning("MainWindow::removeTracker(): No tracker in list selected! m_selectedTrackerInList = " + std::to_string(m_selectedTrackerInList));

	}

	//set the curser to default
	QApplication::restoreOverrideCursor();

}

// SLOT: get selected tracker index from tracker list
void MainWindow::slotTrackerSelectionChanged()
{

	int previousSelectedTrackerInList = m_selectedTrackerInList;

	QList<QTreeWidgetItem*> selectedList = ui->treeWidget_tracker->selectedItems();

	if (selectedList.count() == 1)
	{

		QTreeWidgetItem* selectedItem = selectedList.at(0);

		if (selectedItem != nullptr)
		{

			// get index of selected tracker
			m_selectedTrackerInList = ui->treeWidget_tracker->indexOfTopLevelItem(selectedItem);

		}

	}



	//#### Correct if no errors occur (17.06.2020) #####

	// update the inspector if current tracker was reselected
	//if (previousSelectedTrackerInList == m_selectedTrackerInList)
	//{

	//	//updateInspector();	
	//	drawInspector();


	//}
	//else // if other tracker than before was selected - draw the ui with new content
	//{

	drawInspector();

	//}

}

void MainWindow::slotInspectorItemChanged(QTableWidgetItem* item)
{
	if (m_isInspectorInit)
	{

		switch (item->row())
		{
		case 3: //isEnabled
		{

			// set the curser to wait circle
			QApplication::setOverrideCursor(Qt::WaitCursor);
			//QApplication::processEvents();

			// enabled or disable tracker based on check state
			if (item->checkState() == Qt::Checked)
			{

				m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->enable();

			}
			else if (item->checkState() == Qt::Unchecked)
			{

				m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->disable();

			}
			// update ui
			update();

			// reset cursor to default arrow
			QApplication::restoreOverrideCursor();
			//QApplication::processEvents();

			break;
		}

		case 5:
		{


			break;
		}

		default:
			break;

		}
	}
}

// SLOT: close window / application
void MainWindow::on_actionExit_triggered()
{
	// close window
	this->close();

}


// SLOT: check/uncheck axes in menu
void MainWindow::on_actionToggle_JointAxes(bool menuValue)
{

	m_osgQtWidget->setFlagForJointAxes(menuValue);

}


// SLOT: check/uncheck stick man rendering in menu
void MainWindow::on_actionToggle_StickManRendering(bool menuValue)
{

	m_osgQtWidget->setFlagForStickManRendering(menuValue);

}


// SLOT: check/uncheck solid bone rendering in menu
void MainWindow::on_actionToggle_SolidBoneRendering(bool menuValue)
{

	m_osgQtWidget->setFlagForSolidBoneRendering(menuValue);

}

// SLOT: check/uncheck tracking confidence spheres in menu
void MainWindow::on_actionToggle_TrackingConfidenceSpheres(bool menuValue)
{

	m_osgQtWidget->setFlagForConfidenceSpheresRendering(menuValue);

}


void MainWindow::slotNetworkSettings()
{

	m_netwokSettingsWindow = new SettingsWindow(m_refTrackerManager->m_networkManager, m_configManager);

	m_netwokSettingsWindow->setModal(true);
	m_netwokSettingsWindow->exec();

}

void MainWindow::slotTimelinePressed(float newValue)
{
	m_refTrackerManager->controlTimeline(true);
	m_timelineActive = false;
	m_refTrackerManager->timelineValueChange(m_mmhTimeline->getValue());

}

void MainWindow::slotTimelineReleased(float newValue)
{
	m_refTrackerManager->controlTimeline(false);
	m_timelineActive = true;
	m_refTrackerManager->timelineValueChange(m_mmhTimeline->getValue());

}

void MainWindow::slotTimelineValueChanged(float newValue)
{
	m_refTrackerManager->timelineValueChange(m_mmhTimeline->getValue());	
	//Console::log("MainWindow::setTimelineValue(): value = " + toString(m_mmhTimeline->getValue()));

}

void MainWindow::slotRecord()
{

	Record();

}

void MainWindow::slotTimelineLableModeChanged(int idx)
{


	switch (idx)
	{
	case 0:
		m_timelineLableState = percentage;
		break;


	case 1:
		m_timelineLableState = elTime;
		break;

	case 2:
		m_timelineLableState = frame;
		break;


	default:
		break;
	}
}

void MainWindow::slotOsgWidgetPressed(osg::Vec2 position2d)
{
	m_cameraManipulatorIsRotating = true;
	m_cameraManipulatorStartPosition = position2d;

	Console::log("MainWindow::slotOsgWidgetPressed(): " + toString((float)position2d.x()) + ", " + toString((float)position2d.y()));


}

void MainWindow::slotOsgWidgetReleased(osg::Vec2 position2d)
{
	m_cameraManipulatorIsRotating = false;

}

void MainWindow::slotOsgWidgetMoved(osg::Vec2 position2d)
{
	if (m_cameraManipulatorIsRotating)
	{
		//m_osgQtWidget->setCameraTransform();
	}
}

void MainWindow::slotTimelinePlay()
{
	m_isTimelinePlaying = ui->pushButton_timeline_play->isChecked();

	Tracker* currTracker = m_refTrackerManager->getFirstTrackerFromType(TrackerManager::mmh);

	if (currTracker == nullptr)
	{
		return;
	}

	//start/stop playing mmh-player
	if (m_isTimelinePlaying)
	{
		//currTracker->enable();

		if (currTracker->isTracking())
		{

			currTracker->setPaused(false);
		}
	}
	else
	{
		//currTracker->disable();


		if (currTracker->isTracking())
		{

			currTracker->setPaused(true);
			//currTracker->stop();
		}
	}
}

void MainWindow::slotLoop()
{
	m_isLooping = !m_isLooping;

	Tracker* currTracker = m_refTrackerManager->getFirstTrackerFromType(TrackerManager::mmh);

	if (currTracker == nullptr)
	{
		return;
	}

	//start/stop playing mmh-player
	if (m_isLooping)
	{
		
		currTracker->setLooping(true);
	}
	else
	{
		currTracker->setLooping(false);
	}

}


#pragma endregion Slots

#pragma region

void MainWindow::slotInspectorInputPosX(QString text)
{
	//m_inputFieldPool.at("posX")->setText(text);

	std::string txt = text.toLocal8Bit().constData();

	float posX;

	try
	{

		posX = std::stof(txt);


	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset x value to float!");
		m_inputFieldPool.at("posX")->setText("0");
		posX = 0.0f;

	}

	Vector3f pos = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->positionOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setPositionOffset(Vector3f(posX, pos.y(), pos.z()));
}

void MainWindow::slotInspectorInputPosY(QString text)
{
	//m_inputFieldPool.at("posY")->setText(text);


	std::string txt = text.toLocal8Bit().constData();

	float posY;

	try
	{

		posY = std::stof(txt);

	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset y value to float!");

		m_inputFieldPool.at("posY")->setText("0");
		posY = 0.0f;

	}

	Vector3f pos = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->positionOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setPositionOffset(Vector3f(pos.x(), posY, pos.z()));
}

void MainWindow::slotInspectorInputPosZ(QString text)
{

	//m_inputFieldPool.at("posZ")->setText(text);

	std::string txt = text.toLocal8Bit().constData();

	float posZ;

	try
	{
		posZ = std::stof(txt);
	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");

		m_inputFieldPool.at("posZ")->setText("0");
		posZ = 0.0f;

	}

	Vector3f pos = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->positionOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setPositionOffset(Vector3f(pos.x(), pos.y(), posZ));
}


void MainWindow::slotInspectorInputRotX(QString text)
{

	//m_inputFieldPool.at("rotX")->setText(text);

	std::string txt = text.toLocal8Bit().constData();

	float rotX;

	try
	{
		rotX = std::stof(txt);
	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("rotX")->setText("0");
		rotX = 0.0f;

	}

	Vector3f rot = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->rotationOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setRotationOffset(Vector3f(rotX, rot.y(), rot.z()));
}

void MainWindow::slotInspectorInputRotY(QString text)
{

	//m_inputFieldPool.at("rotY")->setText(text);


	std::string txt = text.toLocal8Bit().constData();

	float rotY;

	try
	{

		rotY = std::stof(txt);

	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("rotY")->setText("0");
		rotY = 0.0f;

	}

	Vector3f rot = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->rotationOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setRotationOffset(Vector3f(rot.x(), rotY, rot.z()));
}

void MainWindow::slotInspectorInputRotZ(QString text)
{

	//m_inputFieldPool.at("rotZ")->setText(text);


	std::string txt = text.toLocal8Bit().constData();

	float rotZ;

	try
	{

		rotZ = std::stof(txt);

	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("rotZ")->setText("0");
		rotZ = 0.0f;

	}

	Vector3f rot = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->rotationOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setRotationOffset(Vector3f(rot.x(), rot.y(), rotZ));
}


void MainWindow::slotInspectorInputScaleX(QString text)
{

	//m_inputFieldPool.at("scaleX")->setText(text);


	std::string txt = text.toLocal8Bit().constData();

	float scaleX;

	try
	{

		scaleX = std::stof(txt);

	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("scaleX")->setText("0");
		scaleX = 0.0f;

	}

	Vector3f scale = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->scaleOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setScaleOffset(Vector3f(scaleX, scale.y(), scale.z()));
}

void MainWindow::slotInspectorInputScaleY(QString text)
{

	//m_inputFieldPool.at("scaleY")->setText(text);

	std::string txt = text.toLocal8Bit().constData();

	float scaleY;

	try
	{

		scaleY = std::stof(txt);

	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("scaleY")->setText("0");
		scaleY = 0.0f;

	}

	Vector3f scale = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->scaleOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setScaleOffset(Vector3f(scale.x(), scaleY, scale.z()));
}

void MainWindow::slotInspectorInputScaleZ(QString text)
{

	//m_inputFieldPool.at("scaleZ")->setText(text);

	std::string txt = text.toLocal8Bit().constData();

	float scaleZ;

	try
	{
		scaleZ = std::stof(txt);
	}
	catch (const std::exception&)
	{

		Console::logError("MainWindow::slotInspectorInputPosZ(): Error parsing position offset z value to float!");
		m_inputFieldPool.at("scaleZ")->setText("0");
		scaleZ = 0.0f;

	}

	Vector3f scale = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->getProperties()->scaleOffset;

	m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->setScaleOffset(Vector3f(scale.x(), scale.y(), scaleZ));
}


//void MainWindow::slotResetTrackerOffset()
//{
//
//	
//	std::vector<Vector3f> currOffsets = m_refTrackerManager->getTrackerRefAt(m_selectedTrackerInList)->resetOffsets();
//
//	slotInspectorInputPosX(toQString(currOffsets[0].x()));
//	slotInspectorInputPosY(toQString(currOffsets[0].y()));
//	slotInspectorInputPosZ(toQString(currOffsets[0].z()));
//	slotInspectorInputRotX(toQString(currOffsets[1].x()));
//	slotInspectorInputRotY(toQString(currOffsets[1].y()));
//	slotInspectorInputRotZ(toQString(currOffsets[1].z()));
//	slotInspectorInputScaleX(toQString(currOffsets[2].x()));
//	slotInspectorInputScaleY(toQString(currOffsets[2].y()));
//	slotInspectorInputScaleZ(toQString(currOffsets[2].z()));
//
//
//}

void MainWindow::slotModifyTrackerRotations()
{

	m_trackerModWindow = new TrackerModWindow();

	m_trackerModWindow->setModal(true);
	m_trackerModWindow->exec();


}


#pragma endregion InspectorInputSlots

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
		ui->btn_addGroup->setDisabled(true);


	}
	else
	{
		//load start button
		icon.addFile(QStringLiteral(":/ressources/icons/icons8-play-32_converted.png"), QSize(), QIcon::Normal, QIcon::Off);

		//enable add/remove buttons
		ui->btn_addTracker->setDisabled(false);
		ui->btn_removeTracker->setDisabled(false);
		ui->btn_addGroup->setDisabled(false);

	}

	// set icon
	ui->btn_startTracker->setIcon(icon);

}

// toogle icon of start / stop tracking button
void MainWindow::toggleRecButtons()
{

	QIcon icon;

	// if tracking is false set icon to start arrow and enbable add / remove tracker buttons
	if (m_isRecording)
	{
		//load stop button
		icon.addFile(QStringLiteral(":/ressources/icons/circle-xxl.png"), QSize(), QIcon::Normal, QIcon::Off);

		////disable add/remove buttons
		//ui->btn_addTracker->setDisabled(true);
		//ui->btn_removeTracker->setDisabled(true);
		//ui->btn_addGroup->setDisabled(true);


	}
	else
	{
		//load start button
		icon.addFile(QStringLiteral(":/ressources/icons/64px-Location_dot_red.svg.png"), QSize(), QIcon::Normal, QIcon::Off);

		////enable add/remove buttons
		//ui->btn_addTracker->setDisabled(false);
		//ui->btn_removeTracker->setDisabled(false);
		//ui->btn_addGroup->setDisabled(false);

	}

	// set icon
	//ui->btn_Record->setIcon(icon);
	ui->btn_Record->setIcon(icon);

}

#pragma endregion Utils

QString MainWindow::toQString(float value)
{



	QString qstr = QString::fromUtf8(std::to_string(value).c_str());


	return qstr;

}

OsgQtWidget* MainWindow::getRenderWindow()
{

	return m_osgQtWidget;

}

void MainWindow::addTrackerToList(int id)
{

	std::vector<Tracker*> trackerPoolRef = m_refTrackerManager->getPoolTracker();

	for (auto itTracker = trackerPoolRef.begin(); itTracker != trackerPoolRef.end(); itTracker++)
	{

		if ((*itTracker)->getProperties()->id == id)
		{

			//ui->treeWidget_tracker->addItem((*itTracker)->getProperties()->name.c_str());


			//m_refTreeWidgetTracker->addItem((*itTracker)->getProperties()->name.c_str());
			QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(ui->treeWidget_tracker);

			// Set text for item
			topLevelItem->setText(0, (*itTracker)->getProperties()->name.c_str());

			// Add it on our tree as the top item.
			ui->treeWidget_tracker->addTopLevelItem(topLevelItem);

		}

	}

}

void MainWindow::setTimelineValue(float totalTime, int frameIdx, int numFrames)
{

	int percent = (int)round((frameIdx * 100) / numFrames);

	if (m_timelineActive)
	{
		m_mmhTimeline->setMaxValue(numFrames);
		m_mmhTimeline->setValue((frameIdx * 100.0f) / numFrames);
	}

	//also set lable

	std::string currStr;

	switch (m_timelineLableState)
	{
	case percentage:
		currStr = toString(percent) + "%";
		break;

	case elTime:
		//currStr = toString((roundf(totalTime * (float)percent) / 100));
		char chr[10];
		sprintf(chr, "%.2f", totalTime * frameIdx / numFrames);
		currStr = chr;
		currStr += "s";
		break;

	case frame:
		currStr = toString(frameIdx) + "/" + toString(numFrames);
		break;

	default:
		break;
	}

	ui->label_timeline->setText(QString(currStr.c_str()));

}

void MainWindow::saveRecord()
{

	Recorder::instance().stopRecording(&m_recordSaveProgression);

}

void MainWindow::progressionBarThread()
{


}

void MainWindow::Record(bool showProgressionBar)
{
	if (m_refTrackerManager->isTracking())
	{

		if (Recorder::instance().isRecording())
		{
			//start saving progress in new thread
			std::thread* saveThread = new std::thread(&MainWindow::saveRecord, this);
			saveThread->detach();

			//get frame count
			int max = Recorder::instance().getFrameCount();

			//Console::log("MainWindow::slotRecord(): start progress window");

			if (showProgressionBar)
			{

				ui->progressBar_save->setMaximum(max);
				ui->progressBar_save->show();


				//std::thread* progThread = new std::thread(&MainWindow::startProgressBar, this, max, &m_recordSaveProgression, ui->progressBar_save);
				////std::thread* progThread = new std::thread(&MainWindow::startProgressBar, this, max, &m_recordSaveProgression, ui->progressBar_save);
				//progThread->detach();

				startProgressBar(max, &m_recordSaveProgression, ui->progressBar_save);


			}
		}
		else
		{
			Recorder::instance().startRecording();
		}




		m_isRecording = !m_isRecording;
		toggleRecButtons();
	}
	else
	{
		Console::logError("Recording is Playmode only!");
	}
}



void MainWindow::startProgressBar(int maxValue, int* currentValue, QProgressBar* barWidget)
{




	while (*currentValue < Recorder::instance().getFrameCount())
	{

		barWidget->setValue(*currentValue);

	}


	barWidget->hide();
}

QTreeWidget* MainWindow::getTreeWidgetTrackerRef()
{
	return ui->treeWidget_tracker;
}

void MainWindow::setTimelinePlayButton(bool state)
{
	ui->pushButton_timeline_play->setChecked(state);

	m_isTimelinePlaying = state;
}