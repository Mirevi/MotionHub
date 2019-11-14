#pragma once

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/QOpenGLTexture>
#include <Qt3DInput/QMouseEvent>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GlWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GlWidget(QWidget* parent = 0);
	~GlWidget();

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	void rotateBy(int xAngle, int yAngle, int zAngle);
	void setClearColor(const QColor& color);

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
	void makeObject();

	QColor clearColor;
	QPoint lastPos;
	int xRot;
	int yRot;
	int zRot;
	QOpenGLTexture* m_texChecker01;
	QOpenGLShaderProgram* program;
	QOpenGLBuffer vbo;
};

#endif