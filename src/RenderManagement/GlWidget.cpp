#include "GlWidget.h"

GlWidget::GlWidget(QWidget* parent)	: QOpenGLWidget(parent)
{

	// set background color to black
	clearColor = Qt::black;

}

GlWidget::~GlWidget()
{

	// set current opengl context active
	makeCurrent(); // called automatically by paintGL()

	delete m_shaderProgram;

	// diable current opengl contex
	doneCurrent();

}

void GlWidget::initializeGL()
{

	// initialize opengl
	initializeOpenGLFunctions();

	// enable depth test and backface culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// load meshes and shader program
	init();

	// bind shader program to current opengl context
	m_shaderProgram->bind();

}

void GlWidget::init()
{

	createMeshes();
	createShaderProgram();

}

void GlWidget::createMeshes()
{

	// create grid
	m_meshPool.push_back(new Primitive(Primitive::Plane, new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png")))));
	// create cube
	m_meshPool.push_back(new Primitive(Primitive::Cube, new QOpenGLTexture(QImage(QString(":/ressources/images/tex_checker_01.png"))), Vector3(0.0f, 0.1f, 0.0f)));

}

void GlWidget::createShaderProgram()
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
	m_shaderProgram = new QOpenGLShaderProgram;
	// add vertex and frament shader
	m_shaderProgram->addShader(vshader);
	m_shaderProgram->addShader(fshader);
	// bind vertex shader attribute to location / id
	m_shaderProgram->bindAttributeLocation("vertex", 0);
	m_shaderProgram->bindAttributeLocation("texCoord", 1);
	// set fragment shader value to location / id
	m_shaderProgram->setUniformValue("texture", 0);
	// link shaders to shader program
	m_shaderProgram->link();

}

// render loop
void GlWidget::paintGL()
{

	// set background color
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset camera matrix
	m_camera.getMatrix()->setToIdentity();
	// set camera to perspective with current aspect ratio
	m_camera.getMatrix()->perspective(60.0f, ((float)this->width() / this->height()), 0.05f, 10.0f);
	// translate and rotate camera
	m_camera.translate(Vector3(0.0f, -0.5f, -2.0f));
	// rotate camera based on mouse movement
	m_camera.rotate();

	// mesh render loop
	for (auto itMesh = m_meshPool.begin(); itMesh != m_meshPool.end(); itMesh++)
		renderMesh(*itMesh);

}

void GlWidget::renderMesh(Mesh* mesh)
{

	// bind mesh vbo and texture
	mesh->bind();

	// assign matrix to shader programm
	m_shaderProgram->setUniformValue("matrix", (*(m_camera.getMatrix()) * *(mesh->getMatrix())));
	// enable shader program attributes set by bindAttributeLocation()
	m_shaderProgram->enableAttributeArray(0);
	m_shaderProgram->enableAttributeArray(1);
	// set vertex and texture coordinate buffers
	// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
	m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
	m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer

	// draw
	for(int faceIndex = 0; faceIndex < mesh->getFaceCount(); faceIndex++)
		glDrawArrays(GL_TRIANGLE_FAN, faceIndex * 4, 4);

	// release mesh vbo and texture
	mesh->release();

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
		m_camera.addRotation(Vector3(dy, dx, 0));
		
	lastPos = event->pos();

	update();

}

void GlWidget::mouseReleaseEvent(QMouseEvent* /* event */)
{

	emit clicked();

}