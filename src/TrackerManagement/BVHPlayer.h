#pragma once

#include "Tracker.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Timer.h"
#include <math.h>

#include "bvh11.h"

class BVHPlayer : public Tracker
{
public:

	BVHPlayer(int id, NetworkManager* networkManager, ConfigManager* configManager, std::string filePath);

	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
	void start() override;
	/*!
	* executes the stop() method of the base class which sets m_tracking to false
	*/
	void stop() override;
	/*!
	 * stops and closes tracker and camera
	 */
	void destroy() override;

	std::string getTrackerType() override;

	std::vector<Vector3f> resetOffsets() override;

	void controlTime(bool stop);

	void setCurrentFrame(int newValue);

	int getCurrentFramePercent();

private:

	/*!
	 * initializes the camera, must only be called once in the beginning
	 * stop() resets all initialization
	 * \param configDevice configuration parameters, standart: DISABLE_ALL
	 */
	void init() override;
	/*!
	 * tracking loop
	 */
	void update() override;
	/*!
	 * starts tracking by getting the capture result and body frame
	 * calls updateSkeletons(...)
	 */
	void track() override;
	/*!
	 * gets the current skeleton data from the current body frame
	 * pushes new skeleton into the pool or updates existing one
	 * \param body_frame
	 */

	bvh11::BvhObject* m_bvhObject;

	bool m_timelineDragging;

	int m_currFrame;
	int m_frameCount;
	double m_frameTime;
	
	Skeleton* m_currSkeleton;

	std::map<std::string, Joint::JointNames> m_nameTranslationTable;



	std::string m_filePath;


};