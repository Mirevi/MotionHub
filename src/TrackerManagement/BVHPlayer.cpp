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






	//create new skeleton and add it to the pool
	m_skeletonPool.insert({ 0, Skeleton(0) });
	m_currSkeleton = &m_skeletonPool[0];

	//there schould only be one skeleton in the file
	m_properties->countDetectedSkeleton = m_skeletonPool.size();
	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

	Console::log("skeleton added to pool!");



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

	delete m_bvhObject;

	// delete this object
	delete this;
}

void BVHPlayer::init()
{
	
	// Instantiate a BVH object
	m_bvhObject = new bvh11::BvhObject("16_44.bvh");

	m_currFrame = 0;
	m_frameCount = m_bvhObject->frames();
	m_frameTime = m_bvhObject->frame_time();

	std::cout << "#Channels       : " << m_bvhObject->channels().size() << std::endl;
	std::cout << "#Frames         : " << m_bvhObject->frames() << std::endl;
	std::cout << "Frame time      : " << m_bvhObject->frame_time() << std::endl;





	m_nameTranslationTable = std::map<std::string, Joint::JointNames>();
	
	m_nameTranslationTable["Hips"] = Joint::HIPS;
	m_nameTranslationTable["LeftUpLeg"] = Joint::UPLEG_L;
	m_nameTranslationTable["LeftLeg"] = Joint::LEG_L;
	m_nameTranslationTable["LeftFoot"] = Joint::FOOT_L;
	m_nameTranslationTable["LeftToeBase"] = Joint::TOE_L;
	m_nameTranslationTable["RightUpLeg"] = Joint::UPLEG_R;
	m_nameTranslationTable["RightLeg"] = Joint::LEG_R;
	m_nameTranslationTable["RightFoot"] = Joint::FOOT_R;
	m_nameTranslationTable["RightToeBase"] = Joint::TOE_R;
	m_nameTranslationTable["Spine"] = Joint::SPINE;
	m_nameTranslationTable["Spine1"] = Joint::CHEST;
	m_nameTranslationTable["Neck1"] = Joint::NECK;
	m_nameTranslationTable["Head"] = Joint::HEAD;
	m_nameTranslationTable["LeftShoulder"] = Joint::SHOULDER_L;
	m_nameTranslationTable["LeftArm"] = Joint::ARM_L;
	m_nameTranslationTable["LeftForeArm"] = Joint::FOREARM_L;
	m_nameTranslationTable["LeftHand"] = Joint::HAND_L;
	m_nameTranslationTable["RightShoulder"] = Joint::SHOULDER_R;
	m_nameTranslationTable["RightArm"] = Joint::ARM_R;
	m_nameTranslationTable["RightForeArm"] = Joint::FOREARM_R;
	m_nameTranslationTable["RightHand"] = Joint::HAND_R;

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

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

}

void BVHPlayer::track()
{

	//get joint list and count
	auto jointList = m_bvhObject->GetJointList();
	int jointCount = jointList.size();


	//Console::log("BVHPlayer::track(): JOINTCOUNT " + toString(jointCount) + ", id = " + toString(m_currSkeleton->getSid()));


	//loop throuh joints
	for each (auto currJoint in jointList)
	{



		//get current joint type
		Joint::JointNames currType = m_nameTranslationTable[currJoint->name()];

		//Console::log("joint name: " + currJoint->name() + ", type: " + toString(currType));

		if (currType != NULL || currType == Joint::HIPS)
		{
			//get joint pose
			Affine3d currJointTransform = m_bvhObject->GetTransformationRelativeToParent(currJoint, m_currFrame);
			Affine3d currJointTransformGlobal = m_bvhObject->GetTransformation(currJoint, m_currFrame);

			//get joint position and convert to Vec4
			auto pos = currJointTransformGlobal.translation().cast<float>();
			Vector4f position = Vector4f(pos.x(), pos.y(), pos.z(), 1) * 0.1f;


			//get joint rotation and convert to Quaternion
			auto rot = currJointTransform.rotation().cast<float>();
			Quaternionf rotation(rot);


			//Console::log("BVHPlayer::track(): Joint " + std::to_string(currType) + ", position: " + toString(position));


			m_currSkeleton->m_joints[currType] = Joint(position, rotation, Joint::HIGH);
		}
	}

	m_currFrame++;

	if (m_currFrame >= m_frameCount)
	{
		m_currFrame = 0;
	}


	m_isDataAvailable = true;
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