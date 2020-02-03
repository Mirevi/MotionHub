#pragma once

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "ConfigDllExportRenderManagement.h"

#include <iostream>
#include <vector>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/QOpenGLTexture>
#include <Qt3DInput/QMouseEvent>

#include "MotionHubUtil/Vector3.h"
#include "TrackerManagement/TrackerManager.h"

#include "Mesh.h"
#include "Plane.h"
#include "Cube.h"
#include "SkeletonMesh.h"

#include "Camera.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

using Framerate = std::chrono::duration<std::chrono::steady_clock::rep, std::ratio<1, 60>>;

/*!
 * \class GlWidget
 *
 * \brief 3d render context for skeleton visualisation
 *
 * \note derives from QOpenGLWidget
 *
 * \author Eric Jansen
 */
class RenderManagement_DLL_import_export GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	/*!
	 * 
	 * standard constructor
	 * 
	 * /parm trackerManager, parent
	 * 
	 */
	explicit GlWidget(TrackerManager* trackerManager, QWidget* parent = 0);
	~GlWidget();

	/*!
	 * 
	 * updates the size of tracker in the pool
	 * 
	 */
	void updateSkeletonMeshPoolSize();
	/*!
	 * 
	 * updates the count of the skeleton meshes for each tracker
	 * 
	 */
	void updateSkeletonMeshCount();
	/*!
	 * 
	 * updates the skeleton mesh / joint position and rotation
	 * 
	 */
	void updateSkeletonMeshTransform();

signals:
	/*!
	 * 
	 * called when user clicks in render window
	 * 
	 */
	void clicked();

protected:
	/*!
	 * 
	 * initialize open gl
	 * 
	 */
	void initializeGL() override;
	/*!
	 * 
	 * render loop
	 * 
	 */
	void paintGL() override;

	/*!
	 * 
	 * window resize
	 * 
	 * \param width
	 * \param height
	 */
	void resizeGL(int width, int height) override;

	/*!
	 * 
	 * event for mouse press
	 * 
	 * \param event
	 */
	void mousePressEvent(QMouseEvent* event) override;
	/*!
	 * 
	 * event for mouse move
	 * 
	 * \param event
	 */
	void mouseMoveEvent(QMouseEvent* event) override;
	/*!
	 * 
	 * event for mouse release
	 * 
	 * \param event
	 */
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	/*!
	 * 
	 * matrix representing the world coordinate system
	 * 
	 */
	QMatrix4x4 m_worldMatrix;

	/*!
	 * 
	 * initialisation
	 * 
	 */
	void init();

	/*!
	 * 
	 * shader for diffuse texture
	 * 
	 */
	QOpenGLShaderProgram* m_shaderProgram_texture;
	/*!
	 * 
	 * shader for joint confidence with diffuse color
	 * 
	 */
	QOpenGLShaderProgram* m_shaderProgram_confidence;
	/*!
	 * 
	 * creates both shader programs
	 * 
	 */
	void createShaderProgram();

	/*!
	 * 
	 * create static meshes ie the ground plane / grid
	 * 
	 */
	void createMeshes();
	/*!
	 * 
	 * renders mesh
	 * 
	 * \param mesh
	 */
	void renderMesh(Mesh* mesh);
	/*!
	 * 
	 * top level render method for skeleton mesh
	 * 
	 * \param skeletonMesh
	 */
	void renderSkeletonMesh(SkeletonMesh* skeletonMesh);

	/*!
	 * 
	 * ground plane / grid
	 * 
	 */
	Plane* m_meshGrid;

	/*!
	 * 
	 * camera
	 * 
	 */
	Camera m_camera;
	/*!
	 * 
	 * last mouse position
	 * 
	 */
	QPoint lastPos;

	/*!
	 * 
	 * background color of the opengl window
	 * 
	 */
	QColor m_clearColor;

	/*!
	 * 
	 * refference to the trackerManager
	 * 
	 */
	TrackerManager* m_refTrackerManager;

	/*!
	 * 
	 * color components for the background color
	 * 
	 */
	Vector3 m_colorRed, m_colorYellow, m_colorGreen;

	/*!
	 * 
	 * skeleton mesh pool
	 * 
	 * key: tracker id refference
	 * value: skeleton mesh pool
	 * 
	 */
	std::map<int, std::vector<SkeletonMesh>> m_skeletonMeshPool;

};

#endif