#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <iostream>
#include "Console.h"

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
		HEAD,
		NDEF
	};

	//enum for confidence level
	enum JointConfidence
	{
		NONE,
		LOW,
		MEDIUM,
		HIGH
	};

	static std::string toString(JointNames type);

	static JointNames toJointNames(std::string name);


	/*!
	 * default constructor
	 * 
	 */
	Joint();

	/*!
	 * standard constructor
	 * 
	 * \param position
	 * \param rotation
	 * \param confidence
	 */
	Joint(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence = HIGH);

	void setTransform(Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence = HIGH);

	void setPosition(Vector4f newPos);

	/*!
	 * getter for Joints Position Vector
	 * 
	 * \return m_position
	 */
	Vector4f getJointPosition();

	/*!
	 * getter for Joints Rotation Quaternion
	 *
	 * \return m_rotation
	 */
	Quaternionf getJointRotation();

	/*!
	 * getter for Joints confidence value
	 * 
	 * \return m_confidence
	 */
	JointConfidence getJointConfidence();

	void setModifier(Vector3f modifier);

	void setInverted(bool x, bool y, bool z);

	/*!
	 * Returns the Joint Name as const char*
	 *
	 * \return Joint Name
	 */
	static const char* getJointName(JointNames jointName) {
		switch (jointName) {
		case Joint::HIPS:
			return "Hips";
		case Joint::SPINE:
			return "Spine";
		case Joint::CHEST:
			return "Chest";
		case Joint::NECK:
			return "Neck";
		case Joint::SHOULDER_L:
			return "LeftShoulder";
		case Joint::ARM_L:
			return "LeftArm";
		case Joint::FOREARM_L:
			return "LeftForearm";
		case Joint::HAND_L:
			return "LeftHand";
		case Joint::SHOULDER_R:
			return "RightShoulder";
		case Joint::ARM_R:
			return "RightArm";
		case Joint::FOREARM_R:
			return "RightForearm";
		case Joint::HAND_R:
			return "RightHand";
		case Joint::UPLEG_L:
			return "LeftUpperleg";
		case Joint::LEG_L:
			return "LeftLeg";
		case Joint::FOOT_L:
			return "LeftFoot";
		case Joint::TOE_L:
			return "LeftToe";
		case Joint::UPLEG_R:
			return "RightUpperleg";
		case Joint::LEG_R:
			return "RightLeg";
		case Joint::FOOT_R:
			return "RightFoot";
		case Joint::TOE_R:
			return "RightToe";
		case Joint::HEAD:
			return "Head";
		}
	}

private:

	Vector4f m_position;
	Quaternionf m_rotation;
	Vector3f m_modifier;
	bool m_invertX;
	bool m_invertY;
	bool m_invertZ;

	JointConfidence m_confidence = NONE;

};