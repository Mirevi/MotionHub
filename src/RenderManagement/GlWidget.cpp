#include "GlWidget.h"

GlWidget::GlWidget(QWidget* parent)	: QOpenGLWidget(parent)
{

	// set background color to black
	clearColor = Qt::black;

	// set camera start rotation
	m_cameraRotation = Vector3::zero();

}

GlWidget::~GlWidget()
{

	// set current opengl context active
	makeCurrent(); // called automatically by paintGL()

	// destroy grid vbo
	m_vboGrid.destroy();

	// delete grid texture and shader program
	delete tex_grid01;
	delete tex_checker01;
	delete m_program01;

	// diable current opengl contex
	doneCurrent();

}

void GlWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{

	m_cameraRotation.m_xyz.x += xAngle;
	m_cameraRotation.m_xyz.y += yAngle;
	m_cameraRotation.m_xyz.z += zAngle;

	update();

}

void GlWidget::initializeGL()
{

	// initialize opengl
	initializeOpenGLFunctions();

	// enable depth test and backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// load vbo's, shader programs and textures
	load();

	// bind shader program to current opengl context
	m_program01->bind();

}

void GlWidget::load()
{

	loadVbo();
	loadShaderProgram();
	loadTextures();

}

void GlWidget::loadTextures()
{

	// load textures
	tex_grid01 = new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png")));
	tex_checker01 = new QOpenGLTexture(QImage(QString(":/ressources/images/tex_checker_01.png")));

}

void GlWidget::loadShaderProgram()
{

	// create and compile vertex shader
	QOpenGLShader * vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char* vsrc =
		"attribute highp vec4 vertex;\n"
		"attribute mediump vec4 texCoord;\n"
		"varying mediump vec4 texc;\n"
		"uniform mediump mat4 matrix;\n"
		"void main(void)\n"
		"{\n"
		"    gl_Position = matrix * vertex;\n"
		"    texc = texCoord;\n"
		"}\n";
	vshader->compileSourceCode(vsrc);

	// create and compile fragment shader
	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char* fsrc =
		"uniform sampler2D texture;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(texture, texc.st);\n"
		"}\n";
	fshader->compileSourceCode(fsrc);

	// create new shader program
	m_program01 = new QOpenGLShaderProgram;
	// add vertex and frament shader
	m_program01->addShader(vshader);
	m_program01->addShader(fshader);
	// bind vertex shader attribute to location / id
	m_program01->bindAttributeLocation("vertex", 0);
	m_program01->bindAttributeLocation("texCoord", 1);
	// set fragment shader value to location / id
	m_program01->setUniformValue("texture", 0);
	// link shaders to shader program
	m_program01->link();

}

void GlWidget::loadVbo()
{

	// create grid vbo
	createGrid();

	// create cube vbo
	createCube();

}

// render loop
void GlWidget::paintGL()
{

	// set background color
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Vector3 currCameraRotation = m_cameraRotation * MOUSE_SPEED;

	// reset camera matrix
	m_cameraMatrix.setToIdentity();
	// set camera to perspective with current aspect ratio
	m_cameraMatrix.perspective(60.0f, ((float)this->width() / this->height()), 0.1f, 10.0f);
	// translate and rotate camera
	m_cameraMatrix.translate(0.0f, -0.5f, -2.0f);
	// rotate camera based on mouse movement
	m_cameraMatrix.rotate(currCameraRotation.m_xyz.x, 1.0f, 0.0f, 0.0f);
	m_cameraMatrix.rotate(currCameraRotation.m_xyz.y, 0.0f, 1.0f, 0.0f);
	m_cameraMatrix.rotate(currCameraRotation.m_xyz.z, 0.0f, 0.0f, 1.0f);

	// assign camera matrix to shader programm
	m_program01->setUniformValue("matrix", m_cameraMatrix);
	// enable shader program attributes set by bindAttributeLocation()
	m_program01->enableAttributeArray(0);
	m_program01->enableAttributeArray(1);

	// bind grid vbo
	if (m_vboGrid.bind())
	{

		// set vertex and texture coordinate buffers
		// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
		m_program01->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
		m_program01->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer
																								   
		// bind grid texture
		tex_grid01->bind();
		// draw grid
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	}

	// bind cube vbo
	if (m_vboCube.bind())
	{

		// set vertex and texture coordinate buffers
		// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
		m_program01->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
		m_program01->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer

		// translate and rotate camera
		m_cameraMatrix.translate(0.0f, -0.4f, 0.0f);

		// draw 10 cubes
		for (int i = 0; i < 10; i++)
		{

			// translate camera
			m_cameraMatrix.translate(0.0f, 0.5f, 0.0f);
			// refresh shader programm with camera matrix after translation
			m_program01->setUniformValue("matrix", m_cameraMatrix);

			// bind checker texture
			tex_checker01->bind();

			// draw cube
			for (int i = 0; i < 6; ++i)
			{
				glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
			}
		}
	}
}
void GlWidget::resizeGL(int width, int height)
{

	// set viewport size based on opengl widget size
	glViewport(0, 0, width, height);

}

void GlWidget::mousePressEvent(QMouseEvent* event)
{
	lastPos = event->pos();
}

void GlWidget::mouseMoveEvent(QMouseEvent* event)
{

	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) 
	{
		rotateBy(8 * dy, 8 * dx, 0);
	}

	lastPos = event->pos();

}

void GlWidget::mouseReleaseEvent(QMouseEvent* /* event */)
{
	emit clicked();
}

void GlWidget::createGrid()
{

	// plane verts
	static const int verts[1][4][3] = 
	{
		{ { +1, 0, -1 }, { -1, 0, -1 }, { -1, 0, +1 }, { +1, 0, +1 } }
	};

	// vert data
	QVector<GLfloat> vertData;

	// assign verts to vert data
	for (int j = 0; j < 4; ++j) 
	{
		// vertex position
		vertData.append(verts[0][j][0]);
		vertData.append(verts[0][j][1]);
		vertData.append(verts[0][j][2]);
		// texture coordinate
		vertData.append(j == 0 || j == 3);
		vertData.append(j == 0 || j == 1);
	}

	// create vbo
	m_vboGrid.create();
	// bind vbo in order to be used by opengl render contex
	m_vboGrid.bind();
	// set usage pattern to static draw because verts do not change
	m_vboGrid.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// allocate vbo based on vertex data size
	m_vboGrid.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

}

void GlWidget::createCube()
{

	// cube verts
	static const int verts[6][4][3] = {
		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
		{ { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
		{ { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
	};

	// vert data
	QVector<GLfloat> vertData;

	// assign verts to vert data
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 4; ++j) {
			// vertex position
			vertData.append(0.1f * verts[i][j][0]);
			vertData.append(0.1f * verts[i][j][1]);
			vertData.append(0.1f * verts[i][j][2]);
			// texture coordinate
			vertData.append(j == 0 || j == 3);
			vertData.append(j == 0 || j == 1);
		}
	}

	// create vbo
	m_vboCube.create();
	// bind vbo in order to be used by opengl render contex
	m_vboCube.bind();
	// set usage pattern to static draw because verts do not change
	m_vboCube.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// allocate vbo based on vertex data size
	m_vboCube.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

}