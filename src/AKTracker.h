#include "Tracker.h"

class AKTracker : public Tracker
{
public:


private:
	k4a_device_t cam;
	k4a_device_configuration_t configCam;
	k4a_calibration_t calibrationCam;
	k4abt_tracker_t tracker;
	k4abt_tracker_configuration_t configTracker;



};