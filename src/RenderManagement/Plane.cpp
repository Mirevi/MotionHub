#include "Plane.h"

Plane::Plane(QOpenGLTexture* texture)
{

	m_texture = texture;

	// plane verts
	static const int verts[1][4][3] = {
		{ { +1, 0, -1 }, { -1, 0, -1 }, { -1, 0, +1 }, { +1, 0, +1 } }
	};

	// assign verts to vert data
	for (int j = 0; j < 4; ++j)
	{
		// vertex position
		m_vertexData.append(verts[0][j][0]);
		m_vertexData.append(verts[0][j][1]);
		m_vertexData.append(verts[0][j][2]);
		// texture coordinate
		m_vertexData.append(j == 0 || j == 3);
		m_vertexData.append(j == 0 || j == 1);
	}

	init();

}

Plane::~Plane()
{

	

}