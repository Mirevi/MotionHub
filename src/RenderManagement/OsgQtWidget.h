#pragma once
#include "ConfigDllExportRenderManagement.h"

#include <QtWidgets/QApplication>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QTimer>

#include <osgQt/GraphicsWindowQt>

#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include "OsgSkeleton.h"



namespace osgQt {
	class GraphicsWindowQt;
}

class TrackerManager;
class ConfigManager;
class OsgAxesCross;
class OsgLine;

//osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h)
//{
//
//}

class RenderManagement_DLL_import_export OsgQtWidget : public QWidget
{
	Q_OBJECT
public:
	OsgQtWidget(osgQt::GraphicsWindowQt* gw, TrackerManager* trackerManager, ConfigManager* configManager);

	void updateSkeletonMeshPoolSize();
	void updateSkeletonMeshTransform();
	void updateSkeletonMeshCount();
	void toggleJointAxes(bool menuValue);
	void toggleStickManRendering(bool menuValue);
	void toggleSolidBoneRendering(bool menuValue);
	void drawLine(osg::Vec3 start, osg::Vec3 end, osg::Vec4 colorStart, osg::Vec4 colorEnd);

protected:
	virtual void paintEvent(QPaintEvent* event)
	{
		m_viewer.frame();
	}

	osgViewer::Viewer m_viewer;
	osg::ref_ptr< osgGA::TrackballManipulator> m_cameraManipulator;
	QTimer m_timer;
	TrackerManager* m_refTrackerManager;
	ConfigManager* m_configManager;

	osg::ref_ptr<osg::Group> m_sceneRoot;

	/*!
	 *
	 * color components for the background color
	 *
	 */
	osg::Vec4f m_colorRed, m_colorYellow, m_colorGreen;

	std::map<int, std::vector<OsgSkeleton>> m_skeletonMeshPool;

	std::vector<osg::ref_ptr<osg::ShapeDrawable>> m_spheres;
	std::vector<osg::ref_ptr<osg::MatrixTransform>> m_sphereTransforms;

	OsgLine* m_line;
};