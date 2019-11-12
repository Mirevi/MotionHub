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

	switch (m_selectedTrackerIdInDropdown)
	{

		case 0:
			try
			{
				m_refTrackerManager->createTracker(TrackerManager::azureKinect); // create new azure kinect tracker and add tracker to the tracking manager tracker pool
			}
			catch (const std::exception&)
			{
				Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Azure Kinect cannot initialize!");

				QApplication::restoreOverrideCursor();
				QApplication::processEvents();

				// close dialog
				close();

			}
			break;

		default:
			Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Tracker type unkown!");
			break;

	}

	m_refListWidgetTracker->addItem(m_refTrackerManager->getPoolTracker()->rbegin()->second->getProperties()->name.c_str());

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