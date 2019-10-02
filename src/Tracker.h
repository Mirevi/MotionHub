#pragma once

#include "Skeleton.h"
#include <list>

#include "AKTracker.h"

class Tracker
{

public:
	enum StateTracker
	{
		INIT,
		TRACKING,
		PAUSED
	};
	bool tracking;
	enum TypeTracker
	{
		AKTracker,
		XSTracker
	};
	Tracker(TypeTracker type);

private:
	StateTracker state;	
	std::list<Skeleton> skeletons;


	void start();
	void stop();
	virtual void track() = 0;
};