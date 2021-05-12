#pragma once
#include "ConfigDllExportRenderManagement.h"

#include <QtWidgets/QApplication>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QTimer>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include <osgQt/GraphicsWindowQt>

#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>

#include "Mesh.h"
//#include "Plane.h"
//#include "Cube.h"
#include "SkeletonMesh.h"



namespace osgQt {
    class GraphicsWindowQt;
}

class TrackerManager;
class ConfigManager;
class AxesCross;
class Line;

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
    Vector3 m_colorRed, m_colorYellow, m_colorGreen;

    std::map<int, std::vector<SkeletonMesh>> m_skeletonMeshPool;

    std::vector<osg::ref_ptr<osg::ShapeDrawable>> m_spheres;
    std::vector<osg::ref_ptr<osg::MatrixTransform>> m_sphereTransforms;
    AxesCross* m_axesCrossTest;
    Line* m_line;
};