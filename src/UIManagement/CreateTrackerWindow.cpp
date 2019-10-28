#include "CreateTrackerWindow.h"
//#include "createtrackerwindow.h"
#include "ui_createtrackerwindow.h"

CreateTrackerWindow::CreateTrackerWindow(InputManager* inputManager, QListView* listViewTracker, QWidget *parent) : QDialog(parent), ui(new Ui::CreateTrackerWindow)
{

	ui->setupUi(this);

	m_refInputManager = inputManager;
	m_listViewTracker = listViewTracker;

	m_refInputManager->registerButton(2);
}

CreateTrackerWindow::~CreateTrackerWindow()
{
	delete ui;
}

void CreateTrackerWindow::createTracker()
{

	QStringListModel* model = new QStringListModel(this);
	QStringList stringList;// = new QStringList();

	QAbstractItemModel* oldModel = m_listViewTracker->model();

	if(oldModel != nullptr)
	{
		for (size_t i = 0; i < oldModel->rowCount(); ++i)
		{
			stringList << oldModel->index(i, 0).data(Qt::DisplayRole).toString();
		}
	}


	stringList.append(ui->dropdown_tracker->currentText());

	model->setStringList(stringList);

	m_listViewTracker->setModel(model);

	m_refInputManager->setButtonPressed(2, 1);
	close();

}

void CreateTrackerWindow::switchTrackerType(int id)
{

	m_refInputManager->setCurrSelectedTrackerId(id);

}