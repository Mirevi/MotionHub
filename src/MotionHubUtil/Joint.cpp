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

std::string Joint::toString(JointNames type)
{

	switch (type)
	{
	case Joint::HIPS:		
		return "HIPS";
	case Joint::SPINE:
		return "SPINE";
	case Joint::CHEST:
		return "CHEST";
	case Joint::NECK:
		return "NECK";
	case Joint::SHOULDER_L:
		return "SHOULDER_L";
	case Joint::ARM_L:
		return "ARM_L";
	case Joint::FOREARM_L:
		return "FOREARM_L";
	case Joint::HAND_L:
		return "HAND_L";
	case Joint::SHOULDER_R:
		return "SHOULDER_R";
	case Joint::ARM_R:
		return "ARM_R";
	case Joint::FOREARM_R:
		return "FOREARM_R";
	case Joint::HAND_R:
		return "HAND_R";
	case Joint::UPLEG_L:
		return "UPLEG_L";
	case Joint::LEG_L:
		return "LEG_L";
	case Joint::FOOT_L:
		return "FOOT_L";
	case Joint::TOE_L:
		return "TOE_L";
	case Joint::UPLEG_R:
		return "UPLEG_R";
	case Joint::LEG_R:
		return "LEG_R";
	case Joint::FOOT_R:
		return "FOOT_R";
	case Joint::TOE_R:
		return "TOE_R";
	case Joint::HEAD:
		return "HEAD";
	case Joint::NDEF:
		return "NDEF";
	default:
		return "";
	}

}

Joint::JointNames Joint::toJointNames(std::string name)
{
	if (name == "HIPS")
		return Joint::HIPS;
	if (name == "SPINE")
		return Joint::SPINE;
	if (name == "CHEST")
		return Joint::CHEST;
	if (name == "NECK")
		return Joint::NECK;
	if (name == "SHOULDER_L")
		return Joint::SHOULDER_L;
	if (name == "ARM_L")
		return Joint::ARM_L;
	if (name == "FOREARM_L")
		return Joint::FOREARM_L;
	if (name == "HAND_L")
		return Joint::HAND_L;
	if (name == "SHOULDER_R")
		return Joint::SHOULDER_R;
	if (name == "ARM_R")
		return Joint::ARM_R;
	if (name == "FOREARM_R")
		return Joint::FOREARM_R;
	if (name == "HAND_R")
		return Joint::HAND_R;
	if (name == "UPLEG_L")
		return Joint::UPLEG_L;
	if (name == "LEG_L")
		return Joint::LEG_L;
	if (name == "FOOT_L")
		return Joint::FOOT_L;
	if (name == "TOE_L")
		return Joint::TOE_L;
	if (name == "UPLEG_R")
		return Joint::UPLEG_R;
	if (name == "LEG_R")
		return Joint::LEG_R;
	if (name == "FOOT_R")
		return Joint::FOOT_R;
	if (name == "TOE_R")
		return Joint::TOE_R;
	if (name == "HEAD")
		return Joint::HEAD;
	if (name == "NDEF")
		return Joint::NDEF;

	return Joint::NDEF;


}

void Joint::setPosition(Vector4f newPos)
{
	m_position = newPos;
}
