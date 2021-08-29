#include "OsgSphere.h"

#include <osg/Geode>

OsgSphere::OsgSphere()
{
	initialize();

}

OsgSphere::OsgSphere(osg::ref_ptr<osg::Group> nodeToAttachTo) {

	nodeToAttachTo->addChild(this);
	initialize();
}

void OsgSphere::initialize()
{
	m_scale = 0.025f;
	createGeometry();
}


void OsgSphere::createGeometry()
{
	m_sphere = new osg::ShapeDrawable();
	m_sphere->setShape(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), m_scale));
	m_sphere->setColor(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));

	addChild(m_sphere);
}

void OsgSphere::setScale(int scale)
{
	m_scale = scale;
	//TODO4: Implement scaling
}

int OsgSphere::getScale()
{
	return m_scale;
}


OsgSphere::~OsgSphere()
{

}

osg::ShapeDrawable* OsgSphere::getSphereGeometry()
{
	return m_sphere.get();
}