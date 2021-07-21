#pragma once

#include <string>
#include <thread>

#include "QtWidgets/qprogressdialog.h"
#include <QtWidgets/QMainWindow>



inline void startProgressBar(int maxValue, int* currentValue, std::string lable, QWidget* parentWidget/*, std::thread* threadHandle*/)
{
	//create progress bar
	QProgressDialog progress(QString(lable.c_str()), "Cancel", 0, maxValue, parentWidget);
	progress.setWindowModality(Qt::WindowModal);
	//progress.show();

	//while still saving
	while (*currentValue < maxValue)
	{
		//update progress bar
		progress.setValue(*currentValue);

		if (progress.wasCanceled())
			break;
	}

	progress.setValue(*currentValue);

	/*threadHandle->join();*/
}

inline void addTrackerToWidget(int id, TrackerManager* refTrackerManager, QTreeWidget* refTreeWidgetTracker)
{
	//update Tree widget
	std::vector<Tracker*> trackerPoolRef = refTrackerManager->getPoolTracker();

	//for all trackers in pool
	for (auto itTracker = trackerPoolRef.begin(); itTracker != trackerPoolRef.end(); itTracker++)
	{
		//get id
		if ((*itTracker)->getProperties()->id == id)
		{
			//add toplevel item tracker
			//m_refTreeWidgetTracker->addItem((*itTracker)->getProperties()->name.c_str());
			QTreeWidgetItem* topLevelItem = new QTreeWidgetItem(refTreeWidgetTracker);

			// Set text for item
			topLevelItem->setText(0, (*itTracker)->getProperties()->name.c_str());

			// Add it on our tree as the top item.
			refTreeWidgetTracker->addTopLevelItem(topLevelItem);

		}

	}
}

inline void testFunc()
{


}

