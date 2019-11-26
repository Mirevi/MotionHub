#pragma once

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <iostream>
#include <list>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/QOpenGLTexture>
#include <Qt3DInput/QMouseEvent>

#include "MotionHubUtil/Vector3.h"
#include "TrackerManagement/TrackerManager.h"

#include "Mesh.h"
#include "Primitive.h"

#include "Camera.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GlWidget(TrackerManager* trackerManager, QWidget* parent = 0);
	~GlWidget();

signals:
	void clicked();

protected:
	void initializeGL() override;
	void paintGL() override;

	void resizeGL(int width, int height) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	QMatrix4x4 m_worldMatrix;

	void init();

	QOpenGLShaderProgram* m_shaderProgram_texture;
	QOpenGLShaderProgram* m_shaderProgram_confidence;
	void createShaderProgram();

	void createMeshes();
	void renderMesh(Mesh* mesh);

	Primitive* m_meshSkeletonJoint01;
	Primitive* m_meshSkeletonJoint02;
	Primitive* m_meshGrid;

	Camera m_camera;
	QPoint lastPos;

	QColor m_clearColor;

	TrackerManager* m_refTrackerManager;

	Vector3 m_colorRed, m_colorYellow, m_colorGreen;

};

#endif