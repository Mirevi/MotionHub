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

private:

	StateTracker state;	
	std::list<Skeleton> skeletons;

	void start();
	void stop();

	virtual void track();
};