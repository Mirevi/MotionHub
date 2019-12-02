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

	Joint curr_joint = Joint(position, rotation, confidence);
	m_joints.insert({ Joint::HIPS, curr_joint });	

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