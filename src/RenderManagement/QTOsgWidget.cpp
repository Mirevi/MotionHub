//#include "QTOsgWidget.h"
//#include <osgQOpenGL/osgQOpenGLWidget>
//#include <QBoxLayout>
//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//#include <osgUtil/Optimizer>
//#include <osgGA/TrackballManipulator>
//#include <QLabel>
//
//QTOsgWidget::QTOsgWidget(QWidget* parent)  : QWidget(parent)
//{
//    resize(400, 300);
//    QHBoxLayout* pLayout = new QHBoxLayout(this);
//    pLayout->setMargin(0);
//
//    osgQOpenGLWidget* pOsgW = new osgQOpenGLWidget;
//    pLayout->addWidget(pOsgW);
//    connect(pOsgW, SIGNAL(initialized()), this, SLOT(initOsg()));
//}
//
//
//QTOsgWidget::~QTOsgWidget()
//{
//}
//
//
//void QTOsgWidget::initOsg()
//{
//    osgViewer::Viewer* pViewer = ((osgQOpenGLWidget*)sender())->getOsgViewer();
//    pViewer->setCameraManipulator(new osgGA::TrackballManipulator());
//    osg::Node* pNode = osgDB::readNodeFile("boxman.osg");
//    osgUtil::Optimizer optimizer;
//    optimizer.optimize(pNode);
//    pViewer->setSceneData(pNode);
//}