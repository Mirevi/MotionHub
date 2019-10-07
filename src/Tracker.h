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
	std::map<int, Skeleton> poolSkeletons;
	//virtual void createSkeleton();
	//virtual void updateSkeletons(k4abt_frame_t* body_frame);
	//virtual void parseSkeleton(k4abt_skeleton_t* skeleton);

private:

	StateTracker state;

};