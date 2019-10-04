#include "Skeleton.h"

Skeleton::Skeleton()
{
	m_state = NONE;


}

Skeleton::Skeleton(int sid)
{
	


}

void Skeleton::updateAge()
{



};

void Skeleton::parseJoint(Joint::jointNames name, Vector3 position, Vector4 rotation)
{
	Joint curr_joint = Joint(position, rotation);
	m_joints.insert({ Joint::JOINT_PELVIS, curr_joint });	
}