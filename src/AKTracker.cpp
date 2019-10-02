#include "AKTracker.h"

//constructor
AKTracker::AKTracker()
{
	


}

void AKTracker::track()
{
	cam = NULL;
	VERIFY(k4a_device_open(0, &cam), "Open K4A Device failed!");

	// Start camera. Make sure depth camera is enabled.
	configCam = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	configCam.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	VERIFY(k4a_device_start_cameras(cam, &configCam), "Start K4A cameras failed!");

	calibrationCam;
	VERIFY(k4a_device_get_calibration(cam, configCam.depth_mode, configCam.color_resolution, &calibrationCam),
		"Get depth camera calibration failed!");

	tracker = NULL;
	configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;
	VERIFY(k4abt_tracker_create(&calibrationCam, configTracker, &tracker), "Body tracker initialization failed!");

	int frame_count = 0;
	do
	{
		k4a_capture_t sensor_capture;
		k4a_wait_result_t get_capture_result = k4a_device_get_capture(cam, &sensor_capture, K4A_WAIT_INFINITE);
		if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
		{
			frame_count++;
			k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, K4A_WAIT_INFINITE);
			k4a_capture_release(sensor_capture); // Remember to release the sensor capture once you finish using it
			if (queue_capture_result == K4A_WAIT_RESULT_TIMEOUT)
			{
				// It should never hit timeout when K4A_WAIT_INFINITE is set.
				printf("Error! Add capture to tracker process queue timeout!\n");
				break;
			}
			else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
			{
				printf("Error! Add capture to tracker process queue failed!\n");
				break;
			}

			k4abt_frame_t body_frame = NULL;
			k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
			if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
			{
				// Successfully popped the body tracking result. Start your processing

				size_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
				printf("%zu bodies are detected!\n", num_bodies);

				k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it
			}
			else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
			{
				//  It should never hit timeout when K4A_WAIT_INFINITE is set.
				printf("Error! Pop body frame result timeout!\n");
				break;
			}
			else
			{
				printf("Pop body frame result failed!\n");
				break;
			}
		}
		else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			// It should never hit time out when K4A_WAIT_INFINITE is set.
			printf("Error! Get depth frame time out!\n");
			break;
		}
		else
		{
			printf("Get depth capture returned error: %d\n", get_capture_result);
			break;
		}

	} while (tracking);

	printf("Finished body tracking processing!\n");

	k4abt_tracker_shutdown(tracker);
	k4abt_tracker_destroy(tracker);
	k4a_device_stop_cameras(cam);
	k4a_device_close(cam);
}