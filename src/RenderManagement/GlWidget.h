#pragma once

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/QOpenGLTexture>
#include <Qt3DInput/QMouseEvent>
#include <iostream>
#include "MotionHubUtil/Vector3.h"
#include "Mesh.h"
#include "Plane.h"
#include "Cube.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

#define MOUSE_SPEED 0.1f

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
	void load();
	void loadTextures();
	void loadShaderProgram();

	int bindVbo(Mesh mesh);

	QColor clearColor;

	QMatrix4x4 m_cameraMatrix;
	Vector3 m_cameraRotation;

	QOpenGLBuffer m_vbo;

	QOpenGLTexture* tex_grid01;
	QOpenGLTexture* tex_checker01;
	QOpenGLShaderProgram* m_program01;

	QPoint lastPos;
	void rotateBy(int xAngle, int yAngle, int zAngle);

	Plane m_meshGrid;
	Cube m_meshCube;

};

#endif