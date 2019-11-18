#include "Primitive.h"

Primitive::Primitive(Type type, QOpenGLTexture* texture, Vector3 position)
{

	switch (type)
	{

		case Plane:
		{
			// plane verts
			int vertsPlane[1][4][3] = {
				{ { +1, 0, -1 }, { -1, 0, -1 }, { -1, 0, +1 }, { +1, 0, +1 } }
			};

			// assign verts to vert data
			for (int j = 0; j < 4; ++j)
			{
				// vertex position
				m_vertexData.append(vertsPlane[0][j][0]);
				m_vertexData.append(vertsPlane[0][j][1]);
				m_vertexData.append(vertsPlane[0][j][2]);
				// texture coordinate
				m_vertexData.append(j == 0 || j == 3);
				m_vertexData.append(j == 0 || j == 1);
			}

			break;
		}

		case Cube:
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
					m_vertexData.append(0.1f * vertsCube[i][j][0]);
					m_vertexData.append(0.1f * vertsCube[i][j][1]);
					m_vertexData.append(0.1f * vertsCube[i][j][2]);
					// texture coordinate
					m_vertexData.append(j == 0 || j == 3);
					m_vertexData.append(j == 0 || j == 1);
				}
			}

			break;
		}

		default:
			break;

	}

	m_texture = texture;

	init();

	translate(position);

}

Primitive::~Primitive()
{



}