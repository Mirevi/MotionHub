#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLBuffer>

class Mesh
{

	public:
		Mesh();
		~Mesh();
		QOpenGLBuffer m_vbo;

	private:
		QVector<GLfloat> m_vertexData;

};