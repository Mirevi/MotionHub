#include "Mesh.h"

Mesh::Mesh()
{



}

Mesh::Mesh(QOpenGLTexture* texture)
{

	m_texture = texture;

}

Mesh::~Mesh()
{



}

QVector<GLfloat> Mesh::getVertData()
{

	return m_vertexData;

}

QOpenGLTexture* Mesh::getTexture()
{

	return m_texture;

}