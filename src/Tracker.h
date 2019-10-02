#pragma once

#include <iostream>
#include <future>

#include "Skeleton.h"
#include <list>

class Tracker
{

public:

	Tracker();
	bool m_tracking = false;

	enum StateTracker
	{
		INIT,
		TRACKING,
		PAUSED
	};

	void start();
	void stop();
	virtual void track();

private:

	StateTracker state;	
	std::list<Skeleton> skeletons;
};