#include "BVHPlayer.h"

BVHPlayer::BVHPlayer(int id, NetworkManager* networkManager, ConfigManager* configManager)
{
	
	m_networkManager = networkManager;
	m_configManager = configManager;

	init();

	//create new Properties object
	m_properties = new Properties();


	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_bvhPlayer_" + toString(id);

	//tracker is enabled
	m_properties->isEnabled = true;


	//set default values for offsets
	setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosBVH"),
							   configManager->getFloatFromStartupConfig("yPosBVH"),
							   configManager->getFloatFromStartupConfig("zPosBVH")
							  ));

	setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotBVH"),
							   configManager->getFloatFromStartupConfig("yRotBVH"),
							   configManager->getFloatFromStartupConfig("zRotBVH")
							  ));

	setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclBVH"),
							configManager->getFloatFromStartupConfig("ySclBVH"),
							configManager->getFloatFromStartupConfig("zSclBVH")
							));


}

void BVHPlayer::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&BVHPlayer::update, this);
	m_trackingThread->detach();

}

void BVHPlayer::stop()
{
	//is not tracking, so the update loop exits after current loop
	m_properties->isTracking = false;
}

void BVHPlayer::destroy()
{
	// delete this object
	delete this;
}

void BVHPlayer::init()
{
	// Instantiate a BVH object
	//m_bvhObject = bvh11::BvhObject("16_44.bvh");
	bvh11::BvhObject bvh("16_44.bvh");

}

void BVHPlayer::update()
{

	// track while tracking is true
	while (m_properties->isTracking)
	{


		// get new data
		track();

		//send Skeleton Pool to NetworkManager
		m_networkManager->sendSkeletonPool(&m_skeletonPool, m_properties->id);

	}

	//clean skeleton pool after tracking
	clean();

}

void BVHPlayer::track()
{

	//important tracking code

}

std::string BVHPlayer::getTrackerType()
{
	return "BVH";
}

std::vector<Vector3f> BVHPlayer::resetOffsets()
{
	Vector3f pos = Vector3f(0, 0, 0);
	Vector3f rot = Vector3f(0, 0, 0);
	Vector3f scl = Vector3f(1, 1, 1);

	setPositionOffset(pos);
	setRotationOffset(rot);
	setScaleOffset(scl);

	std::vector<Vector3f> offsets = { pos, rot, scl };

	return offsets;
}