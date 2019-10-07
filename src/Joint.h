#pragma once

#include <iostream>
#include "Vector3.h"
#include "Vector4.h"

class Joint
{

private:
	std::string m_key;
	Vector3 m_position;
	Vector4 m_rotation;

public:
	Joint();
	Joint(std::string key);
	Vector3 getJointPosition() { return m_position; };
	Vector4 getJointRotation() { return m_rotation; };

	//enum for joint names
	enum jointNames
	{
		JOINT_PELVIS,
		JOINT_SPINE_NAVAL,
		JOINT_SPINE_CHEST,
		JOINT_NECK,
		JOINT_CLAVICLE_LEFT,
		JOINT_SHOULDER_LEFT,
		JOINT_ELBOW_LEFT,
		JOINT_WRIST_LEFT,
		JOINT_CLAVICLE_RIGHT,
		JOINT_SHOULDER_RIGHT,
		JOINT_ELBOW_RIGHT,
		JOINT_WRIST_RIGHT,
		JOINT_HIP_LEFT,
		JOINT_KNEE_LEFT,
		JOINT_ANKLE_LEFT,
		JOINT_FOOT_LEFT,
		JOINT_HIP_RIGHT,
		JOINT_KNEE_RIGHT,
		JOINT_ANKLE_RIGHT,
		JOINT_FOOT_RIGHT,
		JOINT_HEAD,
		JOINT_NOSE,
		JOINT_EYE_LEFT,
		JOINT_EAR_LEFT,
		JOINT_EYE_RIGHT,
		JOINT_EAR_RIGHT,
	};

	Joint(Vector3 position, Vector4 rotation);

};