#pragma once

#include <iostream>
#include <future>

#include "Skeleton.h"
#include <list>

/*!
 * \class Tracker
 *
 * \brief Prototype class for implemetation - holds virtual methods
 *
 * \author Kester Evers and Eric Jansen
 */
class Tracker
{

public:
	Tracker();
	bool m_tracking = false;

	void start();
	void stop();
	//captures one frame of body tracking data and saves all data in the skeleton pool
	virtual void track();

	enum StateTracker
	{
		INIT,
		TRACKING,
		PAUSED
	};

	std::map<int, Skeleton> poolSkeletons;

private:
	StateTracker state;

};