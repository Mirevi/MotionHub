#include "Mesh.h"

Mesh::Mesh()
{

	// plane verts
	static const int verts[1][4][3] =
	{
		{ { +1, 0, -1 }, { -1, 0, -1 }, { -1, 0, +1 }, { +1, 0, +1 } }
	};

	// assign verts to vert data
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < 4; ++j)
		{
			// vertex position
			m_vertexData.append(verts[i][j][0]);
			m_vertexData.append(verts[i][j][1]);
			m_vertexData.append(verts[i][j][2]);
			// texture coordinate
			m_vertexData.append(j == 0 || j == 3);
			m_vertexData.append(j == 0 || j == 1);
		}
	}

	// create vbo
	m_vbo.create();
	// bind vbo in order to be used by opengl render contex
	m_vbo.bind();
	// allocate vbo based on vertex data size
	m_vbo.allocate(m_vertexData.constData(), m_vertexData.count() * sizeof(GLfloat));

}

Mesh::~Mesh()
{

	m_vbo.destroy();

}