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

#include "Mesh.h"
//#include "Plane.h"
//#include "Cube.h"
#include "SkeletonMesh.h"

namespace osgQt {
    class GraphicsWindowQt;
}

class TrackerManager;

//osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h)
//{
//
//}

class RenderManagement_DLL_import_export OsgQtWidget : public QWidget
{
    Q_OBJECT
public:
    OsgQtWidget(osgQt::GraphicsWindowQt* gw, osg::Node* scene, TrackerManager* trackerManager);

    void updateSkeletonMeshPoolSize();
    void updateSkeletonMeshTransform();
    void updateSkeletonMeshCount();

protected:
    virtual void paintEvent(QPaintEvent* event)
    {
        _viewer.frame();
    }

    osgViewer::Viewer _viewer;
    QTimer _timer;
    TrackerManager* m_refTrackerManager;
    
    /*!
     *
     * color components for the background color
     *
     */
    Vector3 m_colorRed, m_colorYellow, m_colorGreen;

    std::map<int, std::vector<SkeletonMesh>> m_skeletonMeshPool;
};