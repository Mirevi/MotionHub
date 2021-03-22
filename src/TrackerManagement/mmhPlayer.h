#pragma once

#include "Tracker.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Timer.h"
#include <math.h>
#include "Recorder.h"


class mmhPlayer : public Tracker
{
public:

	mmhPlayer(int id, NetworkManager* networkManager, ConfigManager* configManager, std::string filePath);

	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
	void start() override;
	/*!
	* executes the stop() method of the base class which sets m_tracking to false
	*/
	void stop() override;

	std::string getTrackerType() override;

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
	void init();

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

	//bvh11::BvhObject* m_bvhObject;
	RecordingSession m_session;
	RecordingFrame* m_currFrame;

	bool m_timelineDragging;

	int m_currFrameIdx;
	int m_frameCount;
	
	Skeleton* m_currSkeleton;

	std::map<std::string, Joint::JointNames> m_nameTranslationTable;



	std::string m_filePath;

	Quaternionf convertJointRotation(Quaternionf raw, Joint::JointNames type);


};