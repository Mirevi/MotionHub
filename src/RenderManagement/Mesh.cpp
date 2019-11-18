#include "Mesh.h"

Mesh::Mesh()
{

	init();

}

Mesh::Mesh(QOpenGLTexture* texture)
{

	m_texture = texture;

	init();

}

Mesh::~Mesh()
{



}

void Mesh::init()
{

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
		
}

void Mesh::bind()
{

	m_vbo.bind();
	m_texture->bind();

	//std::cout << "Mesh::init(): Binded mesh vbo and texture." << std::endl;

}