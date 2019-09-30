#include <Joint.h>

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

private:
	int m_sid;
	StateSkeleton m_state;
	int m_ageFrames;
	Joint m_joints[];
};