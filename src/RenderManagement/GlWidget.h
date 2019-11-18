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

#include "Mesh.h"
#include "Primitive.h"

#include "Camera.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GlWidget(QWidget* parent = 0);
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

	QOpenGLShaderProgram* m_shaderProgram;
	void createShaderProgram();

	std::list<Mesh*> m_meshPool;
	void createMeshes();
	void renderMesh(Mesh* mesh);

	Camera m_camera;
	QPoint lastPos;

	QColor clearColor;

};

#endif