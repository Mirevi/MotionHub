#include "OsgBone.h"

OsgBone::OsgBone(osg::ref_ptr<osg::Group> parentNode) : m_parentNode(parentNode)
{
	//Joint* m_startJoint = new Joint();
	//Joint* m_endJoint = new Joint();;
}

OsgBone::~OsgBone()
{
}

void OsgBone::setStartJoint(Joint joint)
{
	m_startJoint = joint;
}

void OsgBone::setEndJoint(Joint joint)
{
	m_endJoint = joint;
}

void OsgBone::setOffsetRotation(osg::Vec3 offset)
{
}

void OsgBone::updateScale(float scale)
{
	//HWM: Decomment the following (is from OsgQtWidget and adapt it to this class
	// After that, try to implement a bone in osgQtWidget with the same effect as already done with the old way

	//osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
	//	itJoint->second.getJointPosition().y(),
	//	itJoint->second.getJointPosition().z())
	//	-
	//	osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
	//		std::next(itJoint)->second.getJointPosition().y(),
	//		std::next(itJoint)->second.getJointPosition().z());
	//float boneLength = boneVector.length();

	//m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
}

void OsgBone::createLeafJoint()
{
}

