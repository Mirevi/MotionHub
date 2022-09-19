#pragma once

#include "Tracker.h"
#include "RemoteCaptury.h"

/*!
 * \class CLTracker
 *
 * \brief Manages CapturyLive Body Tracking
 *
 * \author Manuel Zohlen
 */
class CLTracker : public Tracker {

public:
	/*!
	 * constructor wit initialization
	 * \param id Tracker ID
	 */
	CLTracker(int id, NetworkManager* networkManager, ConfigManager* configManager);
	/*!
	 * default destructor
	 */
	~CLTracker();
	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
	void start() override;
	/*!
	 * sets m_tracking to false, so track() runs for the last time and exits,
	 * after that, the tracking thread is stopped
	 */
	void stop() override;
	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	void track() override;

protected:

	std::string getTrackerType() override;
	
	std::string* m_ipAddress;
	
	int* m_port;
	/*!
	* wether a valid CLTracker already exists. Used to invalidate subsequently created trackers
	*/
	static bool m_validTrackerExists;
	/*!
	 * wether or not the Tracker is connected to CapturyLive
	 */
	bool m_connected = false;
	/*!
	 * connects to CapturyLive
	 */
	bool connect();
	/*!
	 * disconnects from CapturyLive
	 */
	bool disconnect();
	/*!
	 * updates joints if skeleton with new transformation found in pose
	 * \param skeleton the skeleton to update
	 * \param actor the CapturyActor the pose belongs to
	 * \param pose the new CapturyPose
	 */
	void parseSkeleton(Skeleton* skeleton, CapturyActor* actor, CapturyPose* pose);
	/*!
	 * converts the given CapturyJoint's name to a useable format and additionally returns wether the convesion was successful or not
	 */
	std::pair<Joint::JointNames, bool> parseJointName(const CapturyJoint* joint);
	/*!
	 * recovers the given CapturyJoint's default global position as part of the given CapturyActor
	 */
	Vector4f getCapturyJointPosition(CapturyJoint* joint, CapturyActor* actor);
	/*!
	 * recovers the given CapturyJoint's default global rotation as part of the given CapturyActor
	 */
	inline Quaternionf getCapturyJointRotation(CapturyJoint* joint, CapturyActor* actor);
	/*!
	 * converts the given CapturyTransform's position to a useable format
	 */
	inline Vector4f convertCapturyPosition(const CapturyTransform* transform);
	/*!
	 * converts the given CapturyTransform's rotation to a useable format
	 */
	inline Quaternionf convertCapturyEuler(const CapturyTransform* transform);
};

