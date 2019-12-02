#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <iostream>
//#include "Vector3.h"
//#include "Vector4.h"

#include <MotionHubUtil/MMHmath.h>

//using namespace Eigen;

/*!
 * \class Joint
 *
 * \brief Data Structure for Default Joint with position and rotation
 *
 * \author Kester Evers and Eric Jansen
 */
class MotionHubUtil_DLL_import_export Joint
{

public:

	//enum for joint names
	enum JointNames
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

	//enum for confidence level
	enum JointConfidence
	{
		NONE,
		LOW,
		MEDIUM,
		HIGH
	};

	Joint();
	Joint(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence);

	Vector4f getJointPosition();
	Quaternionf getJointRotation();
	JointConfidence getJointConfidence();

private:

	Vector4f m_position;
	Quaternionf m_rotation;
	JointConfidence m_confidence = NONE;

};