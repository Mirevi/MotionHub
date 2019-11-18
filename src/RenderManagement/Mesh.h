#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLTexture>

#include "Entity.h"

class Mesh : public Entity
{

	public:
		Mesh();
		Mesh(QOpenGLTexture* texture);
		~Mesh();
		QVector<GLfloat> getVertData();
		QOpenGLTexture* getTexture();

	protected:
		QVector<GLfloat> m_vertexData;
		QOpenGLTexture* m_texture;

};