#include "Mesh.h"

Mesh::Mesh()
{



}

Mesh::~Mesh()
{

	m_vbo.destroy();
	delete m_texture;

}

void Mesh::init()
{

	// calc vertex count
	m_faceCount = (m_vertexData.count() / 5) / 4;

	// create vbo
	m_vbo.create();
	// bind vbo in order to be used by opengl render contex
	m_vbo.bind();
	// set usage pattern to static draw because verts do not change
	m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// allocate vbo based on vertex data size
	m_vbo.allocate(m_vertexData.constData(), m_vertexData.count() * sizeof(GLfloat));
	// release vbo
	m_vbo.release();

	//std::cout << "Mesh::init(): Initialized mesh." << std::endl;
	//std::cout << "Mesh::init():	Mesh vertex count = " << m_vertexCount << "." << std::endl;	

}

void Mesh::bind()
{

	m_vbo.bind();
	m_texture->bind();

	//std::cout << "Mesh::bind(): Binded mesh vbo and texture." << std::endl;

}

void Mesh::release()
{

	m_vbo.release();
	m_texture->release();

}

int Mesh::getFaceCount()
{

	return m_faceCount;

}

bool Mesh::isActive()
{

	return m_isActive;

}

void Mesh::setActive(bool active)
{

	m_isActive = active;

}