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
	m_startJointOffset = new osg::PositionAttitudeTransform();
	//m_endJointPosAtt = new osg::PositionAttitudeTransform();

	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	startJoint->addChild(m_startJointOffset);
}

//CTOR for Leaf Joint with only one joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, float lengthToVirtualEndJoint) : m_startJoint(startJoint)
{
	//HWM: Logik für Leaf Bone
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	startJoint->addChild(m_boneNode);
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

