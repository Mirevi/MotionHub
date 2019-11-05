#include "CreateTrackerWindow.h"
#include "ui_createtrackerwindow.h"

// default constructor
CreateTrackerWindow::CreateTrackerWindow(InputManager* inputManager, TrackerManager* trackerManager, QListView* listViewTracker, QWidget *parent) : QDialog(parent), ui(new Ui::CreateTrackerWindow)
{

	// setup dialog
	ui->setupUi(this);

	// assign refference to input manager and list view of main window
	m_refInputManager = inputManager;
	m_refTrackerManager = trackerManager;
	m_refListViewTracker = listViewTracker;

	// register buttons of the dialog in input manager
	m_refInputManager->registerButton(2); // button: create tracker
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

	// create new model and list
	QStringListModel* model = new QStringListModel(this);
	QStringList stringList;

	// create old model and assign current model
	QAbstractItemModel* oldModel = m_refListViewTracker->model();

	if(oldModel != nullptr)
	{
		for (size_t i = 0; i < oldModel->rowCount(); ++i)
		{
			stringList << oldModel->index(i, 0).data(Qt::DisplayRole).toString();
		}
	}

	// append new tracker
	stringList.append(ui->dropdown_tracker->currentText());

	// display new list text
	model->setStringList(stringList);
	m_refListViewTracker->setModel(model);

	// set button pressed
	//m_refInputManager->setButtonPressed(2, 1);


	switch (m_selectedTrackerIdInDropdown)
	{

	case 0:
		m_refTrackerManager->createTracker(TrackerManager::AKT); // create new azure kinect tracker and add tracker to the tracking manager tracker pool
		break;

	default:
		Console::logError("MotionHub::checkInput(): Can not create tracker. Tracker type unkown!");
		break;

	}

	// close dialog
	close();

}

// SLOT: get tracker id of current selected tracker type in dropdown list
void CreateTrackerWindow::switchTrackerType(int id)
{

	m_selectedTrackerIdInDropdown = id;

}