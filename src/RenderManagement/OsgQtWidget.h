#pragma once
#include "ConfigDllExportRenderManagement.h"

#include <QtWidgets/QApplication>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QTimer>

#include <osgQt/GraphicsWindowQt>

#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/OrbitManipulator>
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
	void setFlagForJointAxes(bool menuValue);
	void setFlagForStickManRendering(bool menuValue);
	void setFlagForSolidBoneRendering(bool menuValue);
	void setFlagForConfidenceSpheresRendering(bool menuValue);

	void drawLine(osg::Vec3 start, osg::Vec3 end, osg::Vec4 colorStart, osg::Vec4 colorEnd);

	void setCameraTransform(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);



signals:
	void osgWidgetPressed(osg::Vec2 position2d);
	void osgWidgetReleased(osg::Vec2 position2d);
	void osgWidgetMoved(osg::Vec2 position2d);

protected:

	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;


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

	bool m_renderConfidenceSpheres;
	bool m_renderSolidBoneRendering;
	bool m_renderJointAxes;
	bool m_renderStickManRendering;
};