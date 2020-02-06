#pragma once

#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#include <vector>

class TrackerGroup : public Tracker
{

public:

	TrackerGroup();
	TrackerGroup(int id);
	~TrackerGroup();

	void start() override;
	/*!
	 * sets m_tracking to false, so track() runs for the last time and exits,
	 * after that, the tracking thread is stopped
	 */
	void stop() override;
	/*!
	 * resets the Trackers init data
	 */
	void destroy() override;

	void addTacker(Tracker* tracker);

	void removeTrackerAt(int position);

private:

	/*!
	 * empty override method for Tracker::init()
	 * this tracker does initialization in  createClient()
	 */
	void init() override;
	/*!
	 * updade method used for tracker thread
	 */
	void update() override;
	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	void track() override;

	void mergeSkeletons();


	std::vector<Tracker*> m_groupedTracker;


};