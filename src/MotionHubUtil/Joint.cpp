#include "Joint.h"

#include <Core>


Joint::Joint()
{

	Eigen::Matrix4f test;

}

Joint::Joint(Vector3 position, Vector4 rotation, Joint::JointConfidence confidence)
{
	m_position = position;
	m_rotation = rotation;
	m_confidence = confidence;
}

Vector3 Joint::getJointPosition() 
{

	return m_position; 

}

Vector4 Joint::getJointRotation() 
{

	return m_rotation;

}

Joint::JointConfidence Joint::getJointConfidence()
{

	return m_confidence;

}