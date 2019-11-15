#include "GlWidget.h"

GlWidget::GlWidget(QWidget* parent)	: QOpenGLWidget(parent)
{

	// set background color to black
	clearColor = Qt::black;

	// set camera start rotation
	xRot = 0;
	yRot = 0;
	zRot = 0;

}

GlWidget::~GlWidget()
{

	// set current opengl context active
	makeCurrent(); // called automatically by paintGL()

	// destroy grid vbo
	m_vboGrid.destroy();

	// delete grid texture and shader program
	delete tex_grid01;
	delete m_program01;

	// diable current opengl contex
	doneCurrent();

}

void GlWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{

	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;

	update();

}

void GlWidget::initializeGL()
{

	// initialize opengl
	initializeOpenGLFunctions();

	// create grid vbo
	createGrid();

	// enable depth test and backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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
	// bind shader program to current opengl context
	m_program01->bind();

	// load grid texture
	tex_grid01 = new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png")));

}

// render loop
void GlWidget::paintGL()
{

	// set background color
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset camera matrix
	m_cameraMatrix.setToIdentity();
	// set camera to perspective with current aspect ratio
	m_cameraMatrix.perspective(60.0f, ((float)this->width() / this->height()), 0.1f, 10.0f);
	// translate and rotate camera
	m_cameraMatrix.translate(0.0f, -0.75f, -2.0f);
	// rotate camera based on mouse movement
	m_cameraMatrix.rotate(xRot * MOUSE_SPEED, 1.0f, 0.0f, 0.0f);
	m_cameraMatrix.rotate(yRot * MOUSE_SPEED, 0.0f, 1.0f, 0.0f);
	m_cameraMatrix.rotate(zRot * MOUSE_SPEED, 0.0f, 0.0f, 1.0f);

	// assign camera matrix to shader programm
	m_program01->setUniformValue("matrix", m_cameraMatrix);
	// enable shader program attributes set by bindAttributeLocation()
	m_program01->enableAttributeArray(0);
	m_program01->enableAttributeArray(1);
	// set vertex and texture coordinate buffers
	// attributes: vertex start index, vertex data type, vertex start offset, vertex tuple size, data stride length
	m_program01->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
	m_program01->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer

	// bind grid texture
	tex_grid01->bind();
	// draw quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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
	// allocate vbo based on vertex data size
	m_vboGrid.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

}