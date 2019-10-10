#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "k4a/k4a.h"
#include "k4abt.h"

#include "defines.h"
#include "Tracker.h"

#define VERIFY_K4A_FUNCTION(result, error)																\
    if(result != K4A_RESULT_SUCCEEDED)																	\
    {																									\
		printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__);\
        exit(1);																						\
    } 

/*!
 * a tracker class for Azure Kinect by Microsoft
 */
class AKTracker : public Tracker
{

public:

	AKTracker();
	AKTracker(k4a_device_configuration_t configDevice, int idCam);

	void start();
	void stop();

private:

	int m_idCam = 0;

	k4a_device_t cam;
	k4a_device_configuration_t configCam;
	k4a_calibration_t calibrationCam;

	k4abt_tracker_t tracker;
	k4abt_tracker_configuration_t configTracker;

	int m_numBodies;
	int m_idCurrMaxSkeletons = 0;
	
	void init(k4a_device_configuration_t configDevice);

	void track();


	void updateSkeletons(k4abt_frame_t* body_frame);

	/*!
	 parses skeleton data into the trackers skeleton pool
	  
	\param skeleton the skeleton to parse
	\param id the skeletons id
	\return returns the pointer of the skeleton in the pool
	 */
	Skeleton* parseSkeleton(k4abt_skeleton_t* skeleton, int id);
	void cleanSkeletonList(k4abt_frame_t* bodyFrame);

};