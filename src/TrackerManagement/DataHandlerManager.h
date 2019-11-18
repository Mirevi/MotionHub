#pragma once

//#include "NatNetTypes.h"
#include "NatNetClient.h"
//#include "RigidBodyCollection.h"

#include "Tracker.h"
#include "MotionHubUtil/Skeleton.h"
#include "OTTracker.h"

#include <iostream>
#include <list>


class DataHandlerManager
{


public:

	DataHandlerManager(Tracker::Properties* properties);

	void static DataHandler(sFrameOfMocapData* data, void* pUserData);

	sFrameOfMocapData* getData();
	

private:


	static Tracker::Properties* m_properties;


	static sFrameOfMocapData* m_data;

};