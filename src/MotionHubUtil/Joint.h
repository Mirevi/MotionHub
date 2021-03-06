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

private:

	Vector4f m_position;
	Quaternionf m_rotation;
	Vector3f m_modifier;
	bool m_invertX;
	bool m_invertY;
	bool m_invertZ;

	JointConfidence m_confidence = NONE;

};