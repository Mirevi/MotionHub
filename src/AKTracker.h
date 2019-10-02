#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "Tracker.h"
#include "k4a/k4a.h"
#include "k4abt.h"

#include "defines.h"

#define VERIFY_K4A_FUNCTION(result, error)																\
    if(result != K4A_RESULT_SUCCEEDED)																	\
    {																									\
		printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__);\
        exit(1);																						\
    } 

class AKTracker : public Tracker
{
public:
	AKTracker();
	AKTracker(k4a_device_configuration_t configDevice);
	void start();
	void stop();
	void track();

private:
	k4a_device_t cam;
	k4a_device_configuration_t configCam;
	k4a_calibration_t calibrationCam;
	k4abt_tracker_t tracker;
	k4abt_tracker_configuration_t configTracker;
	void init(k4a_device_configuration_t configDevice);

};