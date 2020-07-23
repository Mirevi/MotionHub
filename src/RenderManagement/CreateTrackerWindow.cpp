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

	switch (m_selectedTrackerIdInDropdown)
	{

		case 0:
		{
			// create new azure kinect tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::azureKinect);
			


			break;

		}

		case 1:
		{
			// create new azure kinect tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::optiTrack);
						
			break;

		}

		case 2:
		{
			// create new BVHPlayer tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::bvh);

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