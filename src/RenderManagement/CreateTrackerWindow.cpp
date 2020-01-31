#include "CreateTrackerWindow.h"
#include "ui_createtrackerwindow.h"

// default constructor
CreateTrackerWindow::CreateTrackerWindow(TrackerManager* trackerManager, QListWidget* listWidgetTracker, QWidget *parent) : QDialog(parent), ui(new Ui::CreateTrackerWindow)
{

	// setup dialog
	ui->setupUi(this);

	// assign refference to tracker manager and list view of main window
	m_refTrackerManager = trackerManager;
	m_refListWidgetTracker = listWidgetTracker;

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

	int id = -1;

	switch (m_selectedTrackerIdInDropdown)
	{

		case 0:
		{
			// create new azure kinect tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::azureKinect); 
			


			break;

		}

		case 2:
		{
			// create new azure kinect tracker and add tracker to the tracking manager tracker pool
			id = m_refTrackerManager->createTracker(TrackerManager::optiTrack);
						
			break;

		}

		default:
			Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Tracker type unkown!");
			break;

	}

	std::vector<Tracker*> trackerPoolRef = m_refTrackerManager->getPoolTracker();

	for (auto itTracker = trackerPoolRef.begin(); itTracker != trackerPoolRef.end(); itTracker++)
	{

		if ((*itTracker)->getProperties()->id == id)
		{

			m_refListWidgetTracker->addItem((*itTracker)->getProperties()->name.c_str());

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