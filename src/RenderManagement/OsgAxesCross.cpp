#include "OsgAxesCross.h"
//#include "MotionHubUtil/ConfigManager.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>

OsgAxesCross::OsgAxesCross() 
{
	initialize();

}

OsgAxesCross::OsgAxesCross(osg::ref_ptr<osg::Group> nodeToAttachTo) {

	nodeToAttachTo->addChild(this);
	initialize();
}

void OsgAxesCross::initialize()
{
	m_scale = 1.0;
	m_heightX = 0.1;
	m_heightY = 0.1;
	m_heightZ = 0.1;
	m_thicknessX = 0.005;
	m_thicknessY = 0.005;
	m_thicknessZ = 0.005;

	createGeometry();
}


void OsgAxesCross::createGeometry()
{
	osg::PositionAttitudeTransform* m_originVisTransform = new osg::PositionAttitudeTransform();

	m_originVisTransform->setScale(osg::Vec3d(m_scale, m_scale, m_scale));

	// x-axis
	osg::Cylinder* cylinderX = new osg::Cylinder(osg::Vec3(0.5 * m_heightX, 0.0, 0.0), m_thicknessX, m_heightX);
	cylinderX->setRotation(osg::Quat(osg::PI_2, osg::Vec3(0.0, 1.0, 0.0)));
	osg::ShapeDrawable* cylinderXDrawable = new osg::ShapeDrawable(cylinderX);
	cylinderXDrawable->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));

	osg::Geode* cylXGeode = new osg::Geode;
	cylXGeode->addDrawable(cylinderXDrawable);

	cylXGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	m_originVisTransform->addChild(cylXGeode);

	// y-axis
	osg::Cylinder* cylinderY = new osg::Cylinder(osg::Vec3(0.0, 0.5 * m_heightY, 0.0), m_thicknessY, m_heightY);
	cylinderY->setRotation(osg::Quat(osg::PI_2, osg::Vec3(1.0, 0.0, 0.0)));
	osg::ShapeDrawable* cylinderYDrawable = new osg::ShapeDrawable(cylinderY);
	cylinderYDrawable->setColor(osg::Vec4(0.0, 1.0, 0.0, 1.0));

	osg::Geode* cylYGeode = new osg::Geode;
	cylYGeode->addDrawable(cylinderYDrawable);

	cylYGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	m_originVisTransform->addChild(cylYGeode);

	//z-axis
	osg::Cylinder* cylinderZ = new osg::Cylinder(osg::Vec3(0.0, 0.0, 0.5 * m_heightZ), m_thicknessZ, m_heightZ);
	//cylinderZ->setRotation(osg::Quat(osg::PI_2, osg::Vec3(1.0, 0.0, 0.0)));
	osg::ShapeDrawable* cylinderZDrawable = new osg::ShapeDrawable(cylinderZ);
	cylinderZDrawable->setColor(osg::Vec4(0.0, 0.0, 1.0, 1.0));

	osg::Geode* cylZGeode = new osg::Geode;
	cylZGeode->addDrawable(cylinderZDrawable);

	cylZGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	m_originVisTransform->addChild(cylZGeode);

	addChild(m_originVisTransform);
}

void OsgAxesCross::setScale(int scale)
{
	m_scale = scale;
}

int OsgAxesCross::getScale()
{
	return m_scale;
}


void OsgAxesCross::setHeightX(float height)
{
	m_heightX = height;
}

float OsgAxesCross::getHeightX()
{
	return m_heightX;
}


void OsgAxesCross::setHeightY(float height)
{
	m_heightY = height;
}

float OsgAxesCross::getHeightY()
{
	return m_heightY;
}


void OsgAxesCross::setHeightZ(float height)
{
	m_heightZ = height;
}

float OsgAxesCross::getHeightZ()
{
	return m_heightZ;
}


void OsgAxesCross::setThicknessX(float thickness)
{
	m_thicknessX = thickness;
}

float OsgAxesCross::getThicknessX()
{
	return m_thicknessX;
}


void OsgAxesCross::setThicknessY(float thickness)
{
	m_thicknessY = thickness;
}

float OsgAxesCross::getThicknessY()
{
	return m_thicknessY;
}


void OsgAxesCross::setThicknessZ(float thickness)
{
	m_thicknessZ = thickness;
}

float OsgAxesCross::getThicknessZ()
{
	return m_thicknessZ;
}


OsgAxesCross::~OsgAxesCross()
{

}