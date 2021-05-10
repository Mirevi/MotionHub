#include "Skeleton.h"

Skeleton::Skeleton()
{



}

Skeleton::Skeleton(int sid)
{
	
	m_state = StateSkeleton::NONE;
	m_posture = StatePosture::UNKNOWN;

	m_sid = sid;

}

void Skeleton::parseJoint(Joint::JointNames name, Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence)
{
	//creates Joint with given values
	Joint curr_joint = Joint(position, rotation, confidence);
	//inserts created joint in m_joints
	m_joints.insert({ name, curr_joint });

}

int Skeleton::getSid()
{

	return m_sid;

}

Skeleton::StatePosture Skeleton::getPosture()
{

	return m_posture;

}

void Skeleton::setPosture(Skeleton::StatePosture posture)
{

	m_posture = posture;

}

float Skeleton::getHeight()
{

	return m_height;

}

void Skeleton::setHeight(float height)
{

	m_height = (height * (-1) + 950) / 1000;

}