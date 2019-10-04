#pragma once

#include <Joint.h>
#include <map>

class Skeleton
{

public:
	enum StateSkeleton
	{
		NONE,
		INIT,
		ACTIVE,
		INACTIVE	
	};
	Skeleton();
	Skeleton(int sid);
	void updateAge();
	void parseJoint(Joint::jointNames name, Vector3 position, Vector4 rotation);
	std::map<Joint::jointNames, Joint> m_joints;


private:
	int m_sid;
	StateSkeleton m_state;
	int m_ageFrames;
	//Joint m_joints[26];
};