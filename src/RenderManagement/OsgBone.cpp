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

//CTOR for Leaf Joint with only one joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, float lengthToVirtualEndJoint, osg::Quat rotationOffset) 
	: m_startJoint(startJoint)
{
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);
}

OsgBone::~OsgBone()
{
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
}

void OsgBone::updateScale(float scale)
{
	////HWM: Decomment the following (is from OsgQtWidget and adapt it to this class
	//// After that, try to implement a bone in osgQtWidget with the same effect as already done with the old way
	//osg::Vec3f boneVector = osg::Vec3(m_startJoint.getJointPosition().x(),
	//	m_startJoint.getJointPosition().y(),
	//	m_startJoint.getJointPosition().z())
	//	-
	//	osg::Vec3(m_endJoint.getJointPosition().x(),
	//		m_endJoint.getJointPosition().y(),
	//		m_endJoint.getJointPosition().z());
	//
	//float boneLength = boneVector.length();

	//setScale(osg::Vec3(boneLength, boneLength, boneLength));
}

void OsgBone::createLeafJoint()
{
}

