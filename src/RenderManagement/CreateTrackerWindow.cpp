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

	// Cast dropdown selection to TrackerManager::TrackerType
	TrackerManager::TrackerType trackerType = static_cast<TrackerManager::TrackerType>(m_selectedTrackerIdInDropdown);

	// Is casted TrackerType valid?
	if (trackerType < TrackerManager::TrackerType::group) {

		// Init empty file path
		std::string filePath;

		// Decide if Tracker is a special case and fill file path 
		switch (trackerType) {

		case TrackerManager::bvh:
			// Get file path with caption, dir & filter
			filePath = getFilePath("choose Mocap file.", "C://", "BVH files (*.bvh)");

			// File path invalid?
			if (filePath == std::string()) {
				return;
			}
			break;

		case TrackerManager::mmh:
			// Get file path with caption, dir & filter
			filePath = getFilePath("choose MMH-Recorded file.", "./data", "MMH files (*.mmh)");

			// File path invalid?
			if (filePath == std::string()) {
				return;
			}
			break;
		}
		
		Console::log("CreateTrackerWindow::slotCreateTracker(): Creating Tracker with Tracker Index: " + std::to_string(m_selectedTrackerIdInDropdown));

		try {
			// Create choosen tracker
			id = m_refTrackerManager->createTracker(trackerType, filePath);
		}
		catch (const Exception& exception) {

			// Set the curser to default
			QApplication::restoreOverrideCursor();
			// Close tracker window
			close();

			std::string error = exception.what();

			Console::logError("CreateTrackerWindow::slotCreateTracker(): " + error);
			QMessageBox::critical(this, tr("Can not create Tracker"), tr(error.c_str()));

			return;
		}
		
	}
	else {
		Console::logError("CreateTrackerWindow::slotCreateTracker(): Can not create tracker. Tracker type unkown!");
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

std::string CreateTrackerWindow::getFilePath(const char* caption, const char* dir, const char* filter)
{
	// get file Path from Open File Dialog with caption, dir & filter
	QString filePath = QFileDialog::getOpenFileName(this, caption, dir, tr(filter));

	// is file path valid?
	if (filePath != NULL) {
		// Return file path as utf-8 string
		return filePath.toUtf8().constData();
	}
	else {
		Console::log("CreateTrackerWindow::getFilePath(): empty file path -> close dialog!");

		// Set the curser to default
		QApplication::restoreOverrideCursor();
		// Close tracker window
		close();

		// Return empty string
		return std::string();
	}
}
