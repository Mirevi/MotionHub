#pragma once

//#include "NatNetTypes.h"
#include "NatNetClient.h"
//#include "RigidBodyCollection.h"

#include "Tracker.h"
#include "MotionHubUtil/Skeleton.h"
#include "OTTracker.h"

#include <iostream>


class DataHandlerManager
{


public:

	DataHandlerManager(std::map<int, Skeleton*>* skeletonPool, Tracker::Properties* properties);

	void static DataHandler(sFrameOfMocapData* data, void* pUserData);
	void static extractSkeleton();
	static Skeleton* parseSkeleton(sSkeletonData skeleton, int id);


private:

	static std::map<int, Skeleton*>* m_refSkeletonPool;
	static Tracker::Properties* m_properties;

	static sFrameOfMocapData* m_data;

	
	

};