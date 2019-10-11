#pragma once

#include <Joint.h>
#include <map>

/*!
 * the default skeleton with all Joint poses 
 */
class Skeleton
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
	void parseJoint(Joint::jointNames name, Vector3 position, Vector4 rotation);
	int getSid();

	std::map<Joint::jointNames, Joint> m_joints;

	enum StateSkeleton
	{
		NONE,
		INIT,
		ACTIVE,
		INACTIVE
	};

private:
	int m_sid;
	StateSkeleton m_state;

};