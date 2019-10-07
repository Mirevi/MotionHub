#pragma once

#include <Joint.h>
#include <map>

class Skeleton
{

public:
	Skeleton();
	Skeleton(int sid);

	void parseJoint(Joint::jointNames name, Vector3 position, Vector4 rotation);

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