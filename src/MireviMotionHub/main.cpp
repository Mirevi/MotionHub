#include "MotionHub.h"

int main(int argc, char** argv)
{

	MotionHub* motionHub = new MotionHub(argc, argv);

	// main loop
	while (motionHub->getUiManager()->getMainWindow()->isVisible())
	{

		if (!motionHub->isTracking() /*&& motionHub->getInputManager()->isButtonPressed(MainWindow::startTracking)*/)
		{

			motionHub->start();

		}
	}

	return 0;

}