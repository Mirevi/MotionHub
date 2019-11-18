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

	DataHandlerManager(std::map<int, Skeleton*>* skeletonPool, Tracker::Properties* properties, OTTracker* tracker);

	void static DataHandler(sFrameOfMocapData* data, void* pUserData);


	static sFrameOfMocapData* m_data;
private:

	void static cleanSkeletonPool();
	void static extractSkeleton();
	static Skeleton* parseSkeleton(sSkeletonData skeleton, int id);

	OTTracker* m_tracker;


	static std::map<int, Skeleton*>* m_refSkeletonPool;
	static Tracker::Properties* m_properties;


	

};