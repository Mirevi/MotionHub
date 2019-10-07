#include "Joint.h"

Joint::Joint()
{



}

Joint::Joint(Vector3 position, Vector4 rotation)
{
	m_position = position;
	m_rotation = rotation;
}

Vector3 Joint::getJointPosition() 
{

	return m_position; 

}

Vector4 Joint::getJointRotation() 
{

	return m_rotation;

}