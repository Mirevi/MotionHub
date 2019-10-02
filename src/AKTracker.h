#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "Tracker.h"
#include "k4a/k4a.h"
#include "k4abt.h"

#include "defines.h"

#define VERIFY(result, error)                                                                            \
    if(result != K4A_RESULT_SUCCEEDED)                                                                   \
    {                                                                                                    \
        printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__); \
        exit(1);                                                                                         \
    } 

class AKTracker : public Tracker
{
public:
	AKTracker();


private:
	k4a_device_t cam;
	k4a_device_configuration_t configCam;
	k4a_calibration_t calibrationCam;
	k4abt_tracker_t tracker;
	k4abt_tracker_configuration_t configTracker;
	void track();

};