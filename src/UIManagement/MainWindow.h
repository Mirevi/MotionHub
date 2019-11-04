#pragma once

#include "ConfigDllExportUIManagement.h"
#include <QtWidgets/QMainWindow>
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/InputManager.h"
#include "CreateTrackerWindow.h"
#include "QtCore/qstringlistmodel.h"
#include "QtCore/qstring.h"

namespace Ui
{
	class MainWindow;
}

/*!
 * \class MainWindow
 * 
 * \brief Qt main Window wrapper containing slot methods
 * 
 * \author Eric Jansen and Kester Evers
 * 
 */
class UIManagement_DLL_import_export MainWindow : public QMainWindow
{
	Q_OBJECT
public:

	/*!
	 * default constructor  
	 */
	explicit MainWindow(InputManager* inputManager, QWidget *parent = 0);
	/*!
	 * default destructor 
	 */
	~MainWindow();


private slots:
	/*!
	 * slot for exit button 
	 */
	void on_actionExit_triggered();
	/*!
	 * slot for start/stop button 
	 */
	void startAllTracker();
	/*!
	 * slot for "add" tracker button 
	 */
	void addTracker();
	/*!
	 * slot for "remove" tracker button
	 */
	void removeTracker();

private:
	/*!
	 * qt class generated by the moc
	 */
	Ui::MainWindow *ui;
	/*!
	 * reference to the inputManager
	*/
	InputManager* m_refInputManager;
	/*!
	 * createTracker dialog 
	 */
	CreateTrackerWindow* m_createTrackerWindow;
	/*!
	 * removes tracker from List view widget
	 *  
	 * \param id the id to remove
	 */
	void removeTrackerFromList(int id);
	/*!
	 *  tracking state for changing icon
	 */
	bool m_isTracking = false;

	void toggleIconStartButton();

	void update();

	std::thread* m_updateThread;

	std::map<std::pair<std::string, int>, std::map<int, Skeleton*>*>* m_refTrackerPool;
};