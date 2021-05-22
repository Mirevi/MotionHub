#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "Joint.h"
#include <map>



/*!
 * \class Skeleton
 *
 * \brief Default Skeleton with all Joint poses
 *
 * \author Kester Evers and Eric Jansen
 */
class MotionHubUtil_DLL_import_export Skeleton
{

public:
	/*!
	 * default constructor
	 * creates empty skeleton with state NONE
	 */
	Skeleton();
	/*!
	 * constructor with skeleton id as argument
	 * \param sid skeleton id
	 */
	Skeleton(int sid);

	/*!
	 *  takes the joint poses and saves them in the joint map
	 * \param name name of the joint
	 * \param position
	 * \param rotation
	 */
	void parseJoint(Joint::JointNames name, Vector4f position, Quaternionf rotation, Joint::JointConfidence confidence);

	/*!
	 * getter for skeleton ID
	 * 
	 * \return m_sid
	 */
	int getSid();

	//TODO: Make a setter and getter method
	/*!
	 * map of all skeleton joints with JointNames as keys
	 * 
	 */
	std::map<Joint::JointNames, Joint> m_joints;

	enum StateSkeleton
	{
		NONE,
		INIT,
		ACTIVE,
		INACTIVE
	};

	enum StatePosture
	{
		UNKNOWN,
		STAND,
		SIT,
		CROUCH,
		JUMP,
		LIE
	};

	/*!
	 * getter for posture
	 * 
	 * \return m_posture
	 */
	StatePosture getPosture();

	/*!
	 * setter for posture
	 * 
	 * \param posture new posture
	 */
	void setPosture(StatePosture posture);

	/*!
	 *  getter for skeleton height
	 * 
	 * \return m_height
	 */
	float getHeight();
	void setHeight(float height);

private:

	/*!
	 * skeleton ID
	 * 
	 */
	int m_sid;
	/*!
	 * shows if skeleton data is activly received
	 * 
	 */
	StateSkeleton m_state;
	/*!
	 * skeleton posture
	 * 
	 */
	StatePosture m_posture;
	/*!
	 * the skeleton height (from X/Z-plane to head joint)
	 * 
	 */
	float m_height;
};