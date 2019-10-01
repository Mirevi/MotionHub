#pragma once

#include "Skeleton.h"
#include <list>

class Tracker
{

public:
	Tracker();
	enum StateTracker
	{
		INIT,
		TRACKING,
		PAUSED
	};

private:
	StateTracker state;	
	std::list<Skeleton> skeletons;

	void updateSkeletons();

};