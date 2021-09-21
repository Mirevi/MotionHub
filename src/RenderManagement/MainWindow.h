#pragma once

#include "ConfigDllExportRenderManagement.h"
#include "CreateTrackerWindow.h"
#include "OsgQtWidget.h"
//#include "GlWidget.h"
#include "MotionHubUtil/Console.h"
#include "SettingsWindow.h"
#include "trackermodwindow.h"

#include "QtWidgets/qtreewidget.h"
#include "QtWidgets/qtablewidget.h"
#include "QtWidgets/qlineedit.h"
#include "QtWidgets/qcheckbox.h"
#include "QtCore/qstringlistmodel.h"
#include "QtCore/qstring.h"

#include "mmhTimeline/mmhtimeline.h"

#include <osg/Array>

#include <math.h>
#include <MotionHubUtil/MMHmath.h>

//TODO: rename this as RenderManager or add a RenderManager and incorparate MainWindow

#define GET_VARIABLE_NAME(Variable) (#Variable)


#include <chrono>
#include <atomic>
#include <list>
#include <map>

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
class RenderManagement_DLL_import_export MainWindow : public QMainWindow
{
	Q_OBJECT
public:

	/*!
	 * default constructor  
	 */
	explicit MainWindow(TrackerManager* trackerManager, ConfigManager* configManager, QWidget *parent = 0);
	/*!
	 * default destructor 
	 */
	~MainWindow();

	/*!
	 * updates Inspector and hirachy 
	 */
	void update();

	/*!
	* gets new logs from the Console and displays them in the list item
	*/
	void updateConsole();

	OsgQtWidget* getRenderWindow();

	void setTimelineValue(float time, int frameNum, int numFrames);

	bool m_timelineActive;


	//GlWidget* getOglRenderer();


	void Record(bool showProgressionBar = true);

	QTreeWidget* getTreeWidgetTrackerRef();

	void setTimelinePlayButton(bool state);

private slots:
	/*!
	 * called when user clicks start/stop button 
	 */
	void slotToggleTracking();
	/*!
	 * called when user clicks "add" tracker button 
	 */
	void slotAddTracker();

	void slotAddGroup();

	/*!
	 * called when user clicks "remove" tracker button
	 */
	void slotRemoveTracker();

	/*!
	 *  called when user clicks on File->Exit
	 */
	void on_actionExit_triggered();
	
	/*!
	*  called when user clicks on Menu->View->Show JointAxes
	*/
	void on_actionToggle_JointAxes(bool menuValue);

	/*!
	*  called when user clicks on Menu->View->Show Sick Man Rendering
	*/
	void on_actionToggle_StickManRendering(bool menuValue);

	/*!
	*  called when user clicks on Menu->View->Show Solid Bone Rendering
	*/
	void on_actionToggle_SolidBoneRendering(bool menuValue);

	/*!
	*  called when user clicks on Menu->View->Show Tracking Confidence Spheres
	*/
	void on_actionToggle_TrackingConfidenceSpheres(bool menuValue);




	/*!
	 * called when user clicks on tracker in list 
	 * \param index id of clicked tracker
	 */
	void slotTrackerSelectionChanged();
	/*!
	 * called when user clicks on checkbox in inspector
	 * \param item
	 */
	void slotInspectorItemChanged(QTableWidgetItem *item);

	void slotNetworkSettings();

	void slotInspectorInputPosX(QString text);
	void slotInspectorInputPosY(QString text);
	void slotInspectorInputPosZ(QString text);
	void slotInspectorInputRotX(QString text);
	void slotInspectorInputRotY(QString text);
	void slotInspectorInputRotZ(QString text);
	void slotInspectorInputScaleX(QString text);
	void slotInspectorInputScaleY(QString text);
	void slotInspectorInputScaleZ(QString text);

	//void slotResetTrackerOffset();

	void slotModifyTrackerRotations();

	void slotTimelinePressed(float newValue);
	void slotTimelineReleased(float newValue);
	void slotTimelineValueChanged(float newValue);

	void slotRecord();

	void slotTimelineLableModeChanged(int idx);


	void slotOsgWidgetPressed	(osg::Vec2 position2d);
	void slotOsgWidgetReleased	(osg::Vec2 position2d);
	void slotOsgWidgetMoved		(osg::Vec2 position2d);

	void slotTimelinePlay();
	void slotLoop();



private:
	/*!
	 * qt class generated by the moc
	 */
	Ui::MainWindow *ui;

	OsgQtWidget* m_osgQtWidget;

	mmhTimeline* m_mmhTimeline;
	/*!
	 * createTracker dialog
	 */
	CreateTrackerWindow* m_createTrackerWindow;

	ConfigManager* m_configManager;

	SettingsWindow* m_netwokSettingsWindow;

	TrackerModWindow* m_trackerModWindow;
	

	/*!
	 * reference to the tracker manager 
	 */
	TrackerManager* m_refTrackerManager;

	/*!
	 * map of top level items in hirachy
	 * each tracker has a list of skeletons as children
	 */
	std::map<QTreeWidgetItem*, std::list<QTreeWidgetItem*>> m_hirachyItemPool;

	/*!
	 *  tracking state for changing icon
	 */
	bool m_isTracking = false;

	/*!
	 *  recording state for changing icon
	 */
	bool m_isRecording = false;
	/*!
	 * true when tracker is selected and inspector isn't empty
	 */
	bool m_isInspectorInit = false;
	/*!
	 * id of the currently selected tracker 
	 */
	int m_selectedTrackerInList;

	Tracker* m_selectedTracker;

	/*!
	 *  updates hirachy and items in it
	 */
	void updateHirachy();
	/*!
	 *  creates new rows and items in the inspector
	 */
	void drawInspector();
	/*!
	 *  updates inspector and items in it
	 */
	void updateInspector();
	/*!
	 * clears all inspector content 
	 */
	void clearInspector();
	/*!
	 * 
	 * adds new row with property and value to the inspector
	 * 
	 * \param propertyName name of the property
	 * \param valueName value connected to property
	 */
	void addRowToInspector(std::string propertyName, std::string valueName);
	/*!
	 *  switches icon of the start/stop button to the opposite icon
	 */
	void toggleTrackingButtons();

	void toggleRecButtons();


	std::map<std::string, QLineEdit*> m_inputFieldPool;

	QString toQString(float _float);

	void addTrackerToList(int id);

	enum TimelineLableMode
	{
		percentage,
		elTime,
		frame
	};

	TimelineLableMode m_timelineLableState;

	void saveRecord();

	void progressionBarThread();

	int m_recordSaveProgression;


	void startProgressBar(int maxValue, int* currentValue, QProgressBar* barWidget);

	osg::Vec2 m_cameraManipulatorStartPosition;
	bool m_cameraManipulatorIsRotating;

	bool m_isTimelinePlaying;
	bool m_isLooping;

};
