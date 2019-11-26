#include "Bone.h"

Bone::Bone(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position, Vector3 scalar)
{

	// cube verts
	int vertsCube[6][4][3] = {
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
			m_vertexData.append(vertsCube[i][j][0]);
			m_vertexData.append(vertsCube[i][j][1]);
			m_vertexData.append(vertsCube[i][j][2]);
			// texture coordinate
			m_vertexData.append(j == 0 || j == 3);
			m_vertexData.append(j == 0 || j == 1);
		}
	}

	// assign texture
	m_texture = texture;

	// assign shader program
	m_shaderProgram = shaderProgram;

	// init vbo
	init();

	// scale object
	scale(scalar);
	// translate object to start position
	translate(Vector3(position.m_xyz.x * ((float)1 / scalar.m_xyz.x), position.m_xyz.y * ((float)1 / scalar.m_xyz.y), position.m_xyz.z * ((float)1 / scalar.m_xyz.z)));

}

Bone::~Bone()
{

	// destroy vbo and delete texture
	m_vbo.destroy();
	delete m_texture;

}