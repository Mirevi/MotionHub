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
	m_vbo.destroy();

	// delete grid texture and shader program
	delete m_texture;
	delete m_shaderProgram;

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

	// load meshes and shader program
	load();

	// bind shader program to current opengl context
	m_shaderProgram->bind();

}

void GlWidget::load()
{

	createMeshes();
	createShaderProgram();

}

void GlWidget::createMeshes()
{

	// create grid
	m_meshGrid = new Plane(new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png"))));
	// create cube
	m_meshCube = new Cube(new QOpenGLTexture(QImage(QString(":/ressources/images/tex_checker_01.png"))));

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
	m_viewMatrix.setToIdentity();
	// set camera to perspective with current aspect ratio
	m_viewMatrix.perspective(60.0f, ((float)this->width() / this->height()), 0.05f, 10.0f);
	// translate and rotate camera
	m_viewMatrix.translate(0.0f, -0.5f, -2.0f);
	// rotate camera based on mouse movement
	m_viewMatrix.rotate(m_cameraRotation.m_xyz.x * MOUSE_SPEED, 1.0f, 0.0f, 0.0f);
	m_viewMatrix.rotate(m_cameraRotation.m_xyz.y * MOUSE_SPEED, 0.0f, 1.0f, 0.0f);
	m_viewMatrix.rotate(m_cameraRotation.m_xyz.z * MOUSE_SPEED, 0.0f, 0.0f, 1.0f);

	renderMesh(m_meshGrid);

	// bind cube vbo
	//if (renderMesh(m_meshCube))
	//{

	//	// set vertex and texture coordinate buffers
	//	// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
	//	m_program01->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
	//	m_program01->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer

	//	// translate and rotate camera
	//	m_cameraMatrix.translate(0.0f, -0.4f, 0.0f);

	//	// draw 10 cubes
	//	for (int i = 0; i < 10; i++)
	//	{

	//		// translate camera
	//		m_cameraMatrix.translate(0.0f, 0.5f, 0.0f);
	//		// refresh shader programm with camera matrix after translation
	//		m_program01->setUniformValue("matrix", m_cameraMatrix);

	//		// draw cube
	//		for (int i = 0; i < 6; ++i)
	//		{
	//			glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	//		}
	//	}
	//}
}

void GlWidget::renderMesh(Mesh* mesh)
{

	mesh->bind();

	// assign matrix to shader programm
	m_shaderProgram->setUniformValue("matrix", m_viewMatrix * mesh->getModelMatrix());
	// enable shader program attributes set by bindAttributeLocation()
	m_shaderProgram->enableAttributeArray(0);
	m_shaderProgram->enableAttributeArray(1);
	// set vertex and texture coordinate buffers
	// attributes: vertex attribute location, vertex data type, vertex start offset, vertex tuple size, data stride length
	m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat)); // vertex coordinates buffer
	m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat)); // texture coordinates buffer

	// draw
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