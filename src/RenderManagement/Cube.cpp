#include "Cube.h"

Cube::Cube()
{

	Vector3 scalar = Vector3(0.05f, 0.05f, 0.05f);
	Vector3 position = Vector3::zero();

	initVerts();

	// create vertex shader
	QOpenGLShader * vshader = new QOpenGLShader(QOpenGLShader::Vertex);
	const char* vsrc;

	// create fragment shader
	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
	const char* fsrc;

	// ### CONFIDENCE SHADER PROGRAM ###

	// define vertex shader
	vsrc =
		"attribute highp vec4 vertex;\n"
		"attribute mediump vec4 texCoord;\n"
		"varying mediump vec4 texc;\n"
		"uniform mediump mat4 matrix;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = matrix * vertex;\n"
		"    texc = texCoord;\n"
		"}\n";

	// define fragment shader
	fsrc =
		"uniform sampler2D texture;\n"
		"varying mediump vec4 texc;\n"
		"uniform mediump vec4 color;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(texture, texc.st) * color;\n"
		"}\n";

	// compile vertex shader
	vshader->compileSourceCode(vsrc);
	// compile fragment shader
	fshader->compileSourceCode(fsrc);

	// create confidence shader program
	QOpenGLShaderProgram* shaderProgram_confidence = new QOpenGLShaderProgram;
	// add vertex and frament shader
	shaderProgram_confidence->addShader(vshader);
	shaderProgram_confidence->addShader(fshader);
	// bind vertex shader attribute to location / id
	shaderProgram_confidence->bindAttributeLocation("vertex", 0);
	shaderProgram_confidence->bindAttributeLocation("texCoord", 1);
	// link shaders to shader program
	shaderProgram_confidence->link();

	// assign texture
	m_texture = new QOpenGLTexture(QImage(QString(":/ressources/images/tex_uvChecker_01.png")));

	// assign shader program
	m_shaderProgram = shaderProgram_confidence;

	// init vbo
	init();

	// scale object
	scale(scalar);
	// translate object to start position
	translate(Vector3(position.m_xyz.x * ((float)1 / scalar.m_xyz.x), position.m_xyz.y * ((float)1 / scalar.m_xyz.y), position.m_xyz.z * ((float)1 / scalar.m_xyz.z)));


}

Cube::Cube(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position, Vector3 scalar)
{

	initVerts();

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

void Cube::initVerts()
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

}

Cube::~Cube()
{

	// destroy vbo and delete texture
	m_vbo.destroy();
	delete m_texture;

}