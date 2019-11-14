#include "GlWidget.h"

GlWidget::GlWidget(QWidget* parent)
	: QOpenGLWidget(parent),
	clearColor(Qt::black),
	xRot(0),
	yRot(900),
	zRot(0),
	program(0)
{
}

GlWidget::~GlWidget()
{
	makeCurrent();
	vbo.destroy();
	delete m_texChecker01;
	delete program;
	doneCurrent();
}

void GlWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;
	update();
}

void GlWidget::setClearColor(const QColor& color)
{
	clearColor = color;
	update();
}

void GlWidget::initializeGL()
{
	initializeOpenGLFunctions();

	makeObject();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

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

	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char* fsrc =
		"uniform sampler2D texture;\n"
		"varying mediump vec4 texc;\n"
		"void main(void)\n"
		"{\n"
		"    gl_FragColor = texture2D(texture, texc.st);\n"
		"}\n";
	fshader->compileSourceCode(fsrc);

	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);
	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	program->link();

	program->bind();
	program->setUniformValue("texture", 0);
}

void GlWidget::paintGL()
{
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 m;
	m.perspective(60.0f, ((float)this->width() / this->height()), 0.1f, 10.0f);
	m.translate(0.0f, -0.75f, -2.0f);
	m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
	m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
	m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);

	program->setUniformValue("matrix", m);
	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
	program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

	for (int i = 0; i < 1; ++i) {
		m_texChecker01->bind();
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
}
void GlWidget::resizeGL(int width, int height)
{
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

	if (event->buttons() & Qt::LeftButton) {
		rotateBy(8 * dy, 8 * dx, 0);
	}
	else if (event->buttons() & Qt::RightButton) {
		rotateBy(8 * dy, 0, 8 * dx);
	}
	lastPos = event->pos();
}

void GlWidget::mouseReleaseEvent(QMouseEvent* /* event */)
{
	emit clicked();
}

void GlWidget::makeObject()
{
	static const int coords[6][4][3] = {
		{ { +1, 0, -1 }, { -1, 0, -1 }, { -1, 0, +1 }, { +1, 0, +1 } },
		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
		{ { +1, 0, +1 }, { -1, 0, +1 }, { -1, 0, -1 }, { +1, 0, -1 } },
		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
	};

	m_texChecker01 = new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png")));

	QVector<GLfloat> vertData;
	for (int i = 0; i < 1; ++i) {
		for (int j = 0; j < 4; ++j) {
			// vertex position
			vertData.append(coords[i][j][0]);
			vertData.append(coords[i][j][1]);
			vertData.append(coords[i][j][2]);
			// texture coordinate
			vertData.append(j == 0 || j == 3);
			vertData.append(j == 0 || j == 1);
		}
	}

	vbo.create();
	vbo.bind();
	vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}