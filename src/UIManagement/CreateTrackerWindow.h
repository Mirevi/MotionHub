#pragma once

#include "ConfigDllExportUIManagement.h"
#include <QtWidgets/QDialog>
#include "MotionHubUtil/Console.h"
#include "QtCore/qstringlistmodel.h"
#include "QtCore/qstring.h"
#include "QtWidgets/qlistwidget.h"
#include "TrackerManagement/TrackerManager.h"


namespace Ui {
	class CreateTrackerWindow;
}

/*!
 * 
 * \class CreateTrackerWindow
 * 
 * \brief dialog for creating new trackers
 * 
 * \author Kester Evers and Eric Jansen
 * 
 */
class UIManagement_DLL_import_export CreateTrackerWindow : public QDialog
{
	Q_OBJECT
public:
	/*!
	 * default constructor 
	 */
	explicit CreateTrackerWindow(TrackerManager* trackerManager, QListWidget* listWidgetTracker, QWidget *parent = 0);
	~CreateTrackerWindow();

private slots:
	/*!
	 * slot for button create 
	 */
	void slotCreateTracker();
	/*!
	 * 
	 * slot for dropdown selection
	 * 
	 * \param id selected tracker id
	 */
	void slotSelectTrackerInDropdown(int id);

private:
	/*!
	 * qt class generated by the moc 
	 */
	Ui::CreateTrackerWindow *ui;

	TrackerManager* m_refTrackerManager;
	/*!
	 *  reference to the tracker list in main window
	 */
	QListWidget* m_refListWidgetTracker;

	int m_selectedTrackerIdInDropdown = 0;

};