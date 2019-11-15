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

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

#define MOUSE_SPEED 0.1f

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GlWidget(QWidget* parent = 0);
	~GlWidget();

	void rotateBy(int xAngle, int yAngle, int zAngle);

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
	void createGrid();

	QColor clearColor;
	QPoint lastPos;
	int xRot;
	int yRot;
	int zRot;
	QOpenGLTexture* tex_grid01;
	QOpenGLShaderProgram* m_program01;
	QOpenGLBuffer m_vboGrid;
	QMatrix4x4 m_cameraMatrix;
};

#endif