#pragma once

#include <iostream>
#include "Vector3.h"
#include "Vector4.h"

class Joint
{

private:

	Vector3 m_position;
	Vector4 m_rotation;

public:

	Joint();
	Joint(Vector3 position, Vector4 rotation);

	Vector3 getJointPosition();
	Vector4 getJointRotation();

	//enum for joint names
	enum jointNames
	{
		HIPS,
		SPINE,
		CHEST,
		NECK,
		SHOULDER_L,
		ARM_L,
		FOREARM_L,
		HAND_L,
		SHOULDER_R,
		ARM_R,
		FOREARM_R,
		HAND_R,
		UPLEG_L,
		LEG_L,
		FOOT_L,
		TOE_L,
		UPLEG_R,
		LEG_R,
		FOOT_R,
		TOE_R,
		HEAD
	};

};