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
	QApplication::processEvents();

	int id = -1;

	switch (m_selectedTrackerIdInDropdown)
	{

		case 0:
		{

			id = m_refTrackerManager->createTracker(TrackerManager::azureKinect); // create new azure kinect tracker and add tracker to the tracking manager tracker pool
			
			m_refListWidgetTracker->addItem(m_refTrackerManager->getPoolTracker()->at({"azureKinect", id})->getProperties()->name.c_str());

			break;

		}

		case 2:
		{

			id = m_refTrackerManager->createTracker(TrackerManager::optiTrack); // create new azure kinect tracker and add tracker to the tracking manager tracker pool
			
			m_refListWidgetTracker->addItem(m_refTrackerManager->getPoolTracker()->at({ "optiTrack", id })->getProperties()->name.c_str());
			
			break;

		}

		default:
			Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Tracker type unkown!");
			break;

	}

	//set the curser to default
	QApplication::restoreOverrideCursor();
	QApplication::processEvents();

	// close dialog
	close();

}

// SLOT: get tracker id of current selected tracker type in dropdown list
void CreateTrackerWindow::slotSelectTrackerInDropdown(int id)
{

	m_selectedTrackerIdInDropdown = id;

}