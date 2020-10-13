#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "k4a/k4a.h"
#include "k4abt.h"

#include "Tracker.h"
#include <list>


/*!
 * internal error management function
 * 
 */
#define VERIFY_K4A_FUNCTION(result, error)																\
	if(result != K4A_RESULT_SUCCEEDED)																	\
	{																									\
		printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__);\
		exit(1);																						\
	} 

/*!
 * \class AKTracker
 *
 * \brief Manages Azure Kinect Body Tracking
 *
 * \author Kester Evers and Eric Jansen
 */
class AKTracker : public Tracker
{

public:
	
	/*!
	 * constructor with config
	 * \param idCam the cameras id number
	 */
	AKTracker(int id, int idCam, NetworkManager* networkManager, ConfigManager* configManager);

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

private:


	 
	/*!
	 * k4a camera handle
	 */
	k4a_device_t m_cam;
	/*!
	 * k4a camera configuration parameters
	 */
	k4a_device_configuration_t m_configCam;
	/*!
	 * k4a calibration type
	 */
	k4a_calibration_t m_calibrationCam;
	/*!
	 * k4a body tracking component handle 
	 */
	k4abt_tracker_t m_tracker;
	/*!
	 * k4a tracker configuration parameters
	 */
	k4abt_tracker_configuration_t m_configTracker;

	
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
	void extractSkeleton(k4abt_frame_t* body_frame);
	/*!
	 *convertes k4a skeleton to default skeleton
	*\param skeleton the k4a skeleton to convert
	*\param id the skeletons id
	*\return returns the pointer of the default skeleton in the pool
	 */
	Skeleton* parseSkeleton(k4abt_skeleton_t* skeleton, int id);
	/*!
	 * deletes all old skeletons from the skeleton pool
	 * \param bodyFrame the k4a frame with all skeleton data
	 */
	void cleanSkeletonPool(k4abt_frame_t* bodyFrame);

	/*!
	 * Applies a rotation of 180 degrees around the y-axis on the given quaternion
	 * 
	 * \param value input quaternion
	 * \return output quaternion
	 */
	Quaternionf convertKinectRotation(Quaternionf value);

	/*!
	 * converts the given euler angles to a quaternion
	 * inverts the z-component, because of the different coordinate system
	 * (we guess so; this is how it works)
	 * 
	 * \param euler input euler angles
	 * \return output quaternion
	 */
	Quaternionf azureKinectEulerToQuaternion(Vector3f euler);

};