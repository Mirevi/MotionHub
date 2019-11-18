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
		~Mesh();

		void bind();
		void release();
		int getFaceCount();

		bool isActive();
		void setActive(bool active);

	protected:
		QVector<GLfloat> m_vertexData;
		int m_faceCount;

		QOpenGLBuffer m_vbo;
		QOpenGLTexture* m_texture;

		void init();

		bool m_isActive = true;

};