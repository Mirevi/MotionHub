#include "Mesh.h"

Mesh::Mesh()
{



}

Mesh::~Mesh()
{

	// destroy vbo and delete texture
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

void Mesh::bind(QMatrix4x4 matrix, Vector3 color)
{

	// bind vbo and texture to current opengl context
	m_vbo.bind();
	m_texture->bind();
	m_shaderProgram->bind();

	// assign matrix to shader programm
	m_shaderProgram->setUniformValue("matrix", matrix * m_matrix);
	m_shaderProgram->setUniformValue("color", color.m_xyz.x, color.m_xyz.y, color.m_xyz.z, 1.0f);
	// enable shader program attributes set by bindAttributeLocation()
	m_shaderProgram->enableAttributeArray(0);
	m_shaderProgram->enableAttributeArray(1);
	// set vertex and texture coordinate buffers
	// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
	m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
	m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer
	
	//std::cout << "Mesh::bind(): Binded mesh vbo and texture." << std::endl;

}

void Mesh::release()
{

	// release vbo and texture
	m_vbo.release();
	m_texture->release();
	//m_shaderProgram->release();

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