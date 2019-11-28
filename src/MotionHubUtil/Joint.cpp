#include "Joint.h"




Joint::Joint()
{



}

Joint::Joint(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence)
{
	m_position = position;
	m_rotation = rotation;
	m_confidence = confidence;
}

Vector4f Joint::getJointPosition()
{

	return m_position; 

}

Quaternionf Joint::getJointRotation()
{

	return m_rotation;

}

Joint::JointConfidence Joint::getJointConfidence()
{

	return m_confidence;

}