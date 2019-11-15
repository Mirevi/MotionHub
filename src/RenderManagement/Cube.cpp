#include "Cube.h"

Cube::Cube(QOpenGLTexture* texture)
{

	m_texture = texture;

	// cube verts
	static const int verts[6][4][3] = {
		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
		{ { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
		{ { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
	};

	// assign verts to vert data
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 4; ++j) {
			// vertex position
			m_vertexData.append(0.1f * verts[i][j][0]);
			m_vertexData.append(0.1f * verts[i][j][1]);
			m_vertexData.append(0.1f * verts[i][j][2]);
			// texture coordinate
			m_vertexData.append(j == 0 || j == 3);
			m_vertexData.append(j == 0 || j == 1);
		}
	}
}

Cube::~Cube()
{



}