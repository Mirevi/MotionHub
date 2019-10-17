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

void Skeleton::parseJoint(Joint::JointNames name, Vector3 position, Vector4 rotation)
{

	Joint curr_joint = Joint(position, rotation);
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