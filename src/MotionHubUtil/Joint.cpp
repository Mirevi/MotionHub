#include "Joint.h"




Joint::Joint()
{

	m_modifier = Vector3f(0, 0, 0);
	m_invertX = false;
	m_invertY = false;
	m_invertZ = false;


}

Joint::Joint(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence)
{
	m_position = position;
	m_rotation = rotation;
	m_confidence = confidence;

	m_modifier = Vector3f(0, 0, 0);
	m_invertX = false;
	m_invertY = false;
	m_invertZ = false;
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

void Joint::setTransform(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence)
{
	m_position = position;
	//m_rotation = rotation;
	m_confidence = confidence;


	Vector3f euler = rotation.toRotationMatrix().eulerAngles(0, 1, 2);

	euler += m_modifier;


	float x = m_invertX ? euler.x() : -euler.x();
	float y = m_invertY ? euler.y() : -euler.y();
	float z = m_invertZ ? euler.z() : -euler.z();




	m_rotation = eulerToQuaternion(Vector3f(x, y, z), false);









}

void Joint::setModifier(Vector3f modifier)
{
	m_modifier = modifier;
}

void Joint::setInverted(bool x, bool y, bool z)
{
	
	m_invertX = x;
	m_invertY = y;
	m_invertZ = z;

}


