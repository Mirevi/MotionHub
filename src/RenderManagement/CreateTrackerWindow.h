#pragma once


#include "QtCore/qstringlistmodel.h"
#include "QtCore/qstring.h"
#include "QtWidgets/qlistwidget.h"
#include "QtWidgets/qtreewidget.h"
#include "QtWidgets/qmessagebox.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/qfiledialog.h>



#include "ConfigDllExportRenderManagement.h"
#include "MotionHubUtil/Console.h"
#include "TrackerManagement/TrackerManager.h"
#include "RenderUtils.h"




namespace Ui
{
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
class RenderManagement_DLL_import_export CreateTrackerWindow : public QDialog
{
	Q_OBJECT
public:
	/*!
	 * default constructor 
	 */
	explicit CreateTrackerWindow(TrackerManager* trackerManager, QTreeWidget* listWidgetTracker, QWidget* parent = 0);
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
	QTreeWidget* m_refTreeWidgetTracker;

	/*!
	 * get file Path from Open File Dialog with caption, dir & filter   
	 */
	std::string getFilePath(const char* caption, const char* dir, const char* filter);

	int m_selectedTrackerIdInDropdown = 0;
};