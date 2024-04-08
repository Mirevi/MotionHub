#pragma once

#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#include <vector>

class TrackerGroup : public Tracker
{

public:

	TrackerGroup();
	TrackerGroup(int id, NetworkManager* networkManager, ConfigManager* configManager);

	void start() override;
	/*!
	 * sets m_tracking to false, so track() runs for the last time and exits,
	 * after that, the tracking thread is stopped
	 */
	void stop() override;

	void addTacker(Tracker* tracker);

	void removeTrackerAt(int position);

private:
	std::string getTrackerType();
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