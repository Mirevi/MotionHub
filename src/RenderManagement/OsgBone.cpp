#include "OsgBone.h"
#include <osgDB/ReadFile>

//TODO: Check if the standard CTOR is necessary
OsgBone::OsgBone()
{
}


//CTOR for start and end joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, osg::ref_ptr<osg::PositionAttitudeTransform> endJoint, osg::Quat rotationOffset)
	: m_startJoint(startJoint), m_endJoint(endJoint)
{
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);
}


//CTOR for leaf joint with only one joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, float lengthToVirtualEndJoint, osg::Quat rotationOffset) 
	: m_startJoint(startJoint)
{
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);

	// m_endJoint -> To be defined?
}


OsgBone::~OsgBone()
{
	//Every member is a osg::ref_ptr and should be cleaned by OSG gabarage collector
	//std::cout << "DTOR OsgBone" << std::endl;
}


void OsgBone::setStartJoint(Joint joint)
{
	//m_startJoint = joint;
}

void OsgBone::setEndJoint(Joint joint)
{
	//m_endJoint = joint;
}

void OsgBone::setOffsetRotation(osg::Vec3 offset)
{
}

void OsgBone::updateScale()
{
	//Check if a leaf bine, then do not use the endJoint, because it is not initialized
	if (m_endJoint)
	{
		osg::Vec3f boneVector = m_startJoint->getPosition() - m_endJoint->getPosition();
		float boneLength = boneVector.length();
		m_startJointOffset->setScale(osg::Vec3(boneLength, boneLength, boneLength));
	}
	else
	{
		m_startJointOffset->setScale(osg::Vec3(0.1f, 0.1f, 0.1f));
	}


	//Test for rotation
	//m_startJointOffset;


	//osg::Vec3f dir = osg::Vec3f(0.0, 0.0, 50.0) - m_startJointOffset->getPosition();
	//dir = m_startJointOffset->getAttitude() * dir;
	//osg::Vec3f lineEndPosition = dir + m_startJointOffset->getPosition();

	//osg::Matrix matrix;
	//matrix.makeLookAt(m_startJointOffset->getPosition(), lineEndPosition, osg::Y_AXIS);
	//m_startJointOffset->setAttitude(matrix.getRotate());
}

void OsgBone::updateScale(float scale)
{
	m_startJointOffset->setScale(osg::Vec3(scale, scale, scale));
}

void OsgBone::createLeafJoint()
{
}

