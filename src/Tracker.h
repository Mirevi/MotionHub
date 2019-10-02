#pragma once

#include "Skeleton.h"
#include <list>

class Tracker
{

public:

	Tracker();
	bool tracking;

	enum StateTracker
	{
		INIT,
		TRACKING,
		PAUSED
	};

	void start();
	void stop();

private:

	StateTracker state;	
	std::list<Skeleton> skeletons;

	virtual void track();
};