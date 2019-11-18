#pragma once

#include <iostream>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLBuffer>

#include "Entity.h"

class Mesh : public Entity
{

	public:
		Mesh();
		Mesh(QOpenGLTexture* texture);
		~Mesh();

		void bind();

	protected:
		QVector<GLfloat> m_vertexData;
		QOpenGLTexture* m_texture;
		QOpenGLBuffer m_vbo;

		void init();

};