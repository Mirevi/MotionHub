#pragma once

#include "ConfigReader.h"
//#include "TrackerManager.h"
#include "NetworkManager.h"
#include "Console.h"

class MotionHub
{

private:
	ConfigReader m_configReader;
	//TrackerManager m_trackerManager;
	NetworkManager m_networkManager;

public:
	MotionHub();

};