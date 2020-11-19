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

	float getTotalTime() override;

	int getCurrFrameIdx() override;

	int getFrameCount() override;

	void applyModChange(Joint::JointNames type, Vector3f mod, bool inverted);

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

	/*!
	* Index of the current Frame
	*/
	int m_currFrame;

	/*!
	* Total number of frames in this recording
	*/
	int m_frameCount;

	/*!
	* time of the current frame
	*/
	double m_frameTime;
	
	Skeleton* m_currSkeleton;

	std::map<std::string, Joint::JointNames> m_nameTranslationTable;



	std::string m_filePath;

	Quaternionf convertJointRotation(Quaternionf raw, Joint::JointNames type);


};