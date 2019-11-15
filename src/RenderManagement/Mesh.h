#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLBuffer>

class Mesh
{

	public:
		Mesh();
		~Mesh();
		QVector<GLfloat> getVertData();

	protected:
		QVector<GLfloat> m_vertexData;

};