#include "CreateTrackerWindow.h"
#include "ui_createtrackerwindow.h"

// default constructor
CreateTrackerWindow::CreateTrackerWindow(TrackerManager* trackerManager, QTreeWidget* treeWidgetTracker, QWidget *parent) : QDialog(parent), ui(new Ui::CreateTrackerWindow)
{

	// setup dialog
	ui->setupUi(this);

	// assign refference to tracker manager and list view of main window
	m_refTrackerManager = trackerManager;
	m_refTreeWidgetTracker = treeWidgetTracker;

}

// default destructor
CreateTrackerWindow::~CreateTrackerWindow()
{

	// delete dialog
	delete ui;

}

// SLOT: create new tracker
void CreateTrackerWindow::slotCreateTracker()
{
	//change curser to wait (Hourglass)
	QApplication::setOverrideCursor(Qt::WaitCursor);

	int id = -2;

	Console::log("CreateTrackerWindow::slotCreateTracker(): m_selectedTrackerIdInDropdown = " + toString(m_selectedTrackerIdInDropdown));

	switch (m_selectedTrackerIdInDropdown)
	{
		//Azure Kinect
		case 0:
		{
			// create new azure kinect tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::azureKinect);
			


			break;

		}
		//OptiTrack
		case 1:
		{
			// create new optiTrack tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::optiTrack);
						
			break;

		}
		//BVH
		case 2:
		{



			//open file dialog
			QString filePath = QFileDialog::getOpenFileName(this, "choose Mocap file.", "C://", tr("BVH files (*.bvh)"));


			//BUG: Curser doesn't set to waiting

			// create new BVHPlayer tracker and add tracker to the tracking manager tracker pool
			if (filePath != NULL)
			{

				id = m_refTrackerManager->createTracker(TrackerManager::bvh, filePath.toUtf8().constData());

			}

			break;

		}
		//MMH
		case 4:
		{

			//open file dialog
			QString filePath = QFileDialog::getOpenFileName(this, "choose MMH-Recorded file.", "./data", tr("MMH files (*.mmh)"));

			//BUG: Curser doesn't set to waiting


			// create new BVHPlayer tracker and add tracker to the tracking manager tracker pool
			if (filePath != NULL)
			{

				id = m_refTrackerManager->createTracker(TrackerManager::mmh, filePath.toUtf8().constData());

	

			}

			break;
		}

		default:
			Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Tracker type unkown!");
			break;

	}

	if (id == -1)
	{

		Console::logError("CreateTrackerWindow::slotCreateTracker(): close dialog!");

		//set the curser to default
		QApplication::restoreOverrideCursor();
		close();

		QMessageBox::critical(this, tr("No Camera connected"), tr("Please connect an Azure Kinect Camera to create a corresponding tracker."));

		return;

	}

	std::vector<Tracker*> trackerPoolRef = m_refTrackerManager->getPoolTracker();

	for (auto itTracker = trackerPoolRef.begin(); itTracker != trackerPoolRef.end(); itTracker++)
	{

		if ((*itTracker)->getProperties()->id == id)
		{

			//m_refTreeWidgetTracker->addItem((*itTracker)->getProperties()->name.c_str());
			QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(m_refTreeWidgetTracker);

			// Set text for item
			topLevelItem->setText(0, (*itTracker)->getProperties()->name.c_str());

			// Add it on our tree as the top item.
			m_refTreeWidgetTracker->addTopLevelItem(topLevelItem);

		}

	}


	//set the curser to default
	QApplication::restoreOverrideCursor();

	// close dialog
	close();

}

// SLOT: get tracker id of current selected tracker type in dropdown list
void CreateTrackerWindow::slotSelectTrackerInDropdown(int id)
{

	m_selectedTrackerIdInDropdown = id;

}