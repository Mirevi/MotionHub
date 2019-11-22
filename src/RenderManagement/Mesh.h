#pragma once

#include <iostream>

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/qopenglshaderprogram.h>

#include "Entity.h"

class Mesh : public Entity
{

	public:
		Mesh();
		~Mesh();

		void bind(QMatrix4x4 matrix, Vector3 color);
		void release();
		int getFaceCount();

		bool isActive();
		void setActive(bool active);

	protected:
		QVector<GLfloat> m_vertexData;
		int m_faceCount;

		QOpenGLBuffer m_vbo;
		QOpenGLTexture* m_texture;
		QOpenGLShaderProgram* m_shaderProgram;

		void init();

		bool m_isActive = true;

};