#include "GlWidget.h"

GlWidget::GlWidget(TrackerManager* trackerManager, QWidget* parent)	: QOpenGLWidget(parent)
{

	// set background color to black
	m_clearColor = Qt::black;

	m_colorRed = Vector3(0.75f, 0.0f, 0.0f);
	m_colorYellow = Vector3(0.75f, 0.75f, 0.0f);
	m_colorGreen = Vector3(0.0f, 0.75f, 0.0f);

	m_refTrackerManager = trackerManager;

	// set world scale
	m_worldMatrix.scale(1.0f, 1.0f, 1.0f);

}

GlWidget::~GlWidget()
{

	// set current opengl context active
	makeCurrent(); // called automatically by paintGL()

	// delete all meshes
	delete m_meshGrid;

	// delete shader program
	delete m_shaderProgram_texture;
	delete m_shaderProgram_confidence;

	// diable current opengl contex
	doneCurrent();

}

void GlWidget::initializeGL()
{

	// initialize opengl
	initializeOpenGLFunctions();

	// enable depth test and backface culling and anti aliasing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	// load meshes and shader program
	init();

}

void GlWidget::init()
{

	createShaderProgram();
	createMeshes();

}

void GlWidget::createMeshes()
{

	// create ground plane grid with texture
	m_meshGrid = new Plane(m_shaderProgram_texture, new QOpenGLTexture(QImage(QString(":/ressources/images/tex_grid_10x10.png"))), Vector3::zero(), Vector3(5.0f, 5.0f, 5.0f));

}

void GlWidget::createShaderProgram()
{

	// create vertex shader
	QOpenGLShader * vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	const char* vsrc;

	// create fragment shader
	QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	const char* fsrc;

	// ### TEXTURE SHADER PROGRAM ###

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

	// create texture shader program
	m_shaderProgram_texture = new QOpenGLShaderProgram;
	// add vertex and frament shader
	m_shaderProgram_texture->addShader(vshader);
	m_shaderProgram_texture->addShader(fshader);
	// bind vertex shader attribute to location / id
	m_shaderProgram_texture->bindAttributeLocation("vertex", 0);
	m_shaderProgram_texture->bindAttributeLocation("texCoord", 1);
	// link shaders to shader program
	m_shaderProgram_texture->link();



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
	m_shaderProgram_confidence = new QOpenGLShaderProgram;
	// add vertex and frament shader
	m_shaderProgram_confidence->addShader(vshader);
	m_shaderProgram_confidence->addShader(fshader);
	// bind vertex shader attribute to location / id
	m_shaderProgram_confidence->bindAttributeLocation("vertex", 0);
	m_shaderProgram_confidence->bindAttributeLocation("texCoord", 1);
	// link shaders to shader program
	m_shaderProgram_confidence->link();

}

void GlWidget::updateSkeletonMeshPoolSize()
{

	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	// lock the tracker pool
	m_refTrackerManager->getTrackerPoolLock()->lock();

	// get the trackerRefPoolSize
	int trackerRefPoolSize = m_skeletonMeshPool.size();
	// get the trackerPoolSize
	int trackerPoolSize = trackerTempCopy.size();

	// add new item to the pool if the trackerRefPoolSize is smaller than the trackerPoolSize
	if (trackerRefPoolSize < trackerPoolSize)
	{
		// loop over all tracker in the tracker pool in order to find the missing one
		for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
		{
			// if the current tracker is not an refferenced item in m_skeletonMeshPool add a new one
			if (m_skeletonMeshPool.find((*itTracker)->getProperties()->id) == m_skeletonMeshPool.end())
			{

				Console::log("GlWidget::updateSkeletonMeshPoolSize(): insert");
				
				// add the tracker refference id as a key and create an new SkeletonMesh vector
				m_skeletonMeshPool.insert(std::make_pair((*itTracker)->getProperties()->id, std::vector<SkeletonMesh>()));

			}
		}
	}

	// remove item from the pool if the trackerRefPoolSize is bigger than the trackerPoolSize
	else if (trackerRefPoolSize > trackerPoolSize)
	{
		// remove items till the m_skeletonMeshPool size is same as the trackerTempCopy size
		while (m_skeletonMeshPool.size() > trackerTempCopy.size())
		{

			// loop over all m_skeletonMeshPool items and remove the one which cant be found in trackerTempCopy
			for (auto itRefTracker = m_skeletonMeshPool.begin(); itRefTracker != m_skeletonMeshPool.end(); itRefTracker++)
			{

				bool isTrackerInPool = false;

				for (int indexTrackerCopy = 0; indexTrackerCopy < trackerTempCopy.size(); indexTrackerCopy++)
				{

					if (trackerTempCopy.at(indexTrackerCopy)->getProperties()->id == itRefTracker->first)
					{

						isTrackerInPool = true;
						break;

					}
				}

				if (isTrackerInPool == false)
				{

					m_skeletonMeshPool.erase(itRefTracker->first);
					break;

				}
			}
		}
	}
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}

void GlWidget::updateSkeletonMeshCount()
{
	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();
	// lock the tracker pool
	m_refTrackerManager->getTrackerPoolLock()->lock();

	// loop over all tracker
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
		std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

		// if tracker is active update
		if ((*itTracker)->getProperties()->isTracking)
		{

			// get skeletonPoolSize
			int skeletonPoolSize = skeletonPoolTempCopy.size();
			// get skeletonMeshPoolSize by id of current tracker
			int skeletonMeshPoolSize = m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size();

			// add skeleton mesh to skeleton mesh pool when skeletonMeshPoolSize is smaller than skeletonPoolSize
			if (skeletonMeshPoolSize < skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() < skeletonPoolTempCopy.size())
				{
					// add new skeletonMesh to skeltonMeshPool
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.push_back(SkeletonMesh());

				}
			}
			// remove skeleton mesh from skeleton mesh pool when skeletonMeshPoolSize is bigger than skeletonPoolSize
			else if (skeletonMeshPoolSize > skeletonPoolSize)
			{

				while (m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.size() > skeletonPoolTempCopy.size())
				{
					// remove skeletonMesh from skeletonMeshPool
					m_skeletonMeshPool.find((*itTracker)->getProperties()->id)->second.pop_back();

				}
			}
		}
	}

	// unlock the trackerPool
	m_refTrackerManager->getTrackerPoolLock()->unlock();
}

void GlWidget::updateSkeletonMeshTransform()
{

	// get tracker pool from the tracker manager
	std::vector<Tracker*> trackerTempCopy = m_refTrackerManager->getPoolTracker();

	// loop over all tracker in the pool
	for (auto itTracker = trackerTempCopy.begin(); itTracker != trackerTempCopy.end(); itTracker++)
	{

		// update skeleton joint position and rotation if new data is available
		if ((*itTracker)->isTracking() && (*itTracker)->isDataAvailable())
		{

			// get skeletonPoolCache from tracker and create skeletonPoolTempCopy
			std::map<int, Skeleton> skeletonPoolTempCopy = (*itTracker)->getSkeletonPoolCache();

			int indexSkeleton = 0;

			// update each skeleton
			for (auto itSkeleton = skeletonPoolTempCopy.begin(); itSkeleton != skeletonPoolTempCopy.end(); itSkeleton++)
			{

				int indexJoint = 0;

				// update each joint
				for (auto itJoint = itSkeleton->second.m_joints.begin(); itJoint != itSkeleton->second.m_joints.end(); itJoint++)
				{

					//Console::log("GlWidget::updateSkeletonMeshTransform(): pool size = " + toString(m_skeletonMeshPool.size()));

					// get current skeleton
					auto currMeshTracker = m_skeletonMeshPool.find((*itTracker)->getProperties()->id);

					if (currMeshTracker->second.size() == 0)
					{
						return;
					}

					// get current joint
					Cube* currJoint = currMeshTracker->second.at(indexSkeleton).m_joints[indexJoint];

					// set joint position and rotation
					currJoint->setPosition(itJoint->second.getJointPosition());
					currJoint->setRotation(itJoint->second.getJointRotation());

					// set joint confidence in the shader
					switch (itJoint->second.getJointConfidence())
					{

					case Joint::HIGH:
						currJoint->setDiffuseColor(m_colorGreen);
						break;

					case Joint::MEDIUM:
						currJoint->setDiffuseColor(m_colorYellow);
						break;

					case Joint::LOW:
						currJoint->setDiffuseColor(m_colorRed);
						break;

					case Joint::NONE:
						currJoint->setDiffuseColor(Vector3::one());
						break;

					default:
						break;

					}

					indexJoint++;

				}

				indexSkeleton++;

			}
		}
	}
}

// render loop
void GlWidget::paintGL()
{

	// set background color
	glClearColor(m_clearColor.redF(), m_clearColor.greenF(), m_clearColor.blueF(), m_clearColor.alphaF());
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// reset camera matrix
	m_camera.getMatrix()->setToIdentity();
	// set camera to perspective with current aspect ratio
	m_camera.getMatrix()->perspective(60.0f, ((float)this->width() / this->height()), 0.01f, 25.0f);
	// translate and rotate camera
	m_camera.translate(Vector3(0.0f, -2.0f, -5.0f));
	// rotate camera based on mouse movement
	m_camera.rotate();

	renderMesh(m_meshGrid);

	// skeleton render loop

	for (auto itTracker = m_skeletonMeshPool.begin(); itTracker != m_skeletonMeshPool.end(); itTracker++)
	{
		for (auto itSkeletonMesh = itTracker->second.begin(); itSkeletonMesh != itTracker->second.end(); itSkeletonMesh++)
		{

			// render skeleton mesh
			renderSkeletonMesh(&*itSkeletonMesh);

		}
	}

	// update render window
	update();

}

void GlWidget::renderMesh(Mesh* mesh)
{

	// bind mesh vbo, texture and shader program
	mesh->bind(m_worldMatrix * *(m_camera.getMatrix()));

	// draw all faces with GL_TRIANGLE_FAN
	for(int faceIndex = 0; faceIndex < mesh->getFaceCount(); faceIndex++)
		glDrawArrays(GL_TRIANGLE_FAN, faceIndex * 4, 4);

	// release mesh vbo and texture
	mesh->release();

}

void GlWidget::renderSkeletonMesh(SkeletonMesh* skeletonMesh)
{

	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
		renderMesh(skeletonMesh->m_joints[indexJoint]);

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

	// get x and y differance based on last mouse position
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	// add difference to camera rotation if left mouse button is pressed
	if (event->buttons() & Qt::LeftButton)
		m_camera.addRotation(Vector3(dy, dx, 0));
		
	lastPos = event->pos();

	// update opengl
	update();

}

void GlWidget::mouseReleaseEvent(QMouseEvent* /* event */)
{

	emit clicked();

}
