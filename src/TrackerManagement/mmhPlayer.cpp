#include "mmhPlayer.h"

mmhPlayer::mmhPlayer(int id, NetworkManager* networkManager, ConfigManager* configManager, std::string filePath)
{
	
	m_networkManager = networkManager;
	m_configManager = configManager;

	m_filePath = filePath;

	m_isInitiated = false;
	init();

	//create new Properties object
	m_properties = new Properties();


	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_mmhPlayer_" + toString(id);

	//tracker is enabled
	m_isEnabled = true;


	//set default values for offsets
	//setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosMMH"),
	//						   configManager->getFloatFromStartupConfig("yPosMMH"),
	//						   configManager->getFloatFromStartupConfig("zPosMMH")
	//						  ));

	//setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotMMH"),
	//						   configManager->getFloatFromStartupConfig("yRotMMH"),
	//						   configManager->getFloatFromStartupConfig("zRotMMH")
	//						  ));

	//setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclMMH"),
	//						configManager->getFloatFromStartupConfig("ySclMMH"),
	//						configManager->getFloatFromStartupConfig("zSclMMH")
	//						));
	readOffsetFromConfig();

}

void mmhPlayer::start()
{

	// set tracking to true
	m_isTracking = true;


	////create new skeleton and add it to the pool
	//m_skeletonPool.insert({ 0, Skeleton(0) });
	//m_currSkeleton = &m_skeletonPool[0];

	//there schould only be one skeleton in the file
	m_countDetectedSkeleton = m_skeletonPool.size();
	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;



	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&mmhPlayer::update, this);
	m_trackingThread->detach();

}

void mmhPlayer::stop()
{
	//is not tracking, so the update loop exits after current loop
	m_isTracking = false;
}


void mmhPlayer::init()
{
	m_timelineDragging = false;

	m_session = RecordingSession();

	if (!m_session.load(m_filePath))
	{
		return;
	}

	//Console::log("mmhPlayer::init(): totalTime = " + toString(m_session.getTotalTime()));

	//store metadata
	m_currFrameIdx = 0;
	m_frameCount = m_session.getFrameCount();


	//create lookup table for Joints
	m_nameTranslationTable = std::map<std::string, Joint::JointNames>();
	
	m_nameTranslationTable["Ndef"]			= Joint::NDEF;
	m_nameTranslationTable["LeftUpLeg"]		= Joint::UPLEG_L;
	m_nameTranslationTable["LeftLeg"]		= Joint::LEG_L;
	m_nameTranslationTable["LeftFoot"]		= Joint::FOOT_L;
	m_nameTranslationTable["LeftToeBase"]	= Joint::TOE_L;
	m_nameTranslationTable["RightUpLeg"]	= Joint::UPLEG_R;
	m_nameTranslationTable["RightLeg"]		= Joint::LEG_R;
	m_nameTranslationTable["RightFoot"]		= Joint::FOOT_R;
	m_nameTranslationTable["RightToeBase"]	= Joint::TOE_R;
	m_nameTranslationTable["Spine"]			= Joint::SPINE;
	m_nameTranslationTable["Spine1"]		= Joint::CHEST;
	m_nameTranslationTable["Neck1"]			= Joint::NECK;
	m_nameTranslationTable["Head"]			= Joint::HEAD;
	m_nameTranslationTable["LeftShoulder"]	= Joint::SHOULDER_L;
	m_nameTranslationTable["LeftArm"]		= Joint::ARM_L;
	m_nameTranslationTable["LeftForeArm"]	= Joint::FOREARM_L;
	m_nameTranslationTable["LeftHand"]		= Joint::HAND_L;
	m_nameTranslationTable["RightShoulder"] = Joint::SHOULDER_R;
	m_nameTranslationTable["RightArm"]		= Joint::ARM_R;
	m_nameTranslationTable["RightForeArm"]	= Joint::FOREARM_R;
	m_nameTranslationTable["RightHand"]		= Joint::HAND_R;
	m_nameTranslationTable["Hips"]			= Joint::HIPS;

	m_isInitiated = true;
}

void mmhPlayer::update()
{

	// track while tracking is true
	while (m_isTracking)
	{

		Timer::reset();


		if (!m_timelineDragging)
		{
			
			// get new data
			track();

		}

		m_trackingCycles = 222;

		//send Skeleton Pool to NetworkManager
		m_networkManager->sendSkeletonPool(&getSkeletonPoolCache(), m_properties->id, m_trackingCycles);


		//Recorder::instance().addSkeletonsToFrame(&m_skeletonPool);


		double elapsed = (double)Timer::getDuration();
		long long sleepTime = (m_currFrame->m_duration - elapsed) * 1000;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));


		


	}

	//clean skeleton pool after tracking
	clean();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

}

void mmhPlayer::track()
{

	if (m_currFrameIdx >= m_frameCount)
	{
		m_currFrameIdx = 0;
	}



	m_currFrame = m_session.getFrame(m_currFrameIdx);

	int skelIdx = 0;

	//Console::log("mmhPlayer::track(): skeleton count = " + toString(m_currFrame->m_skeletons.size()));

	//get the current number of skeletons
	m_countDetectedSkeleton = m_currFrame->m_skeletons.size();

	m_skeletonPoolLock.lock();

	if (m_countDetectedSkeleton != m_skeletonPool.size())
	{
		m_hasSkeletonPoolChanged = true;
		m_isDataAvailable = true;

		//Console::log("mmhPlayer::track(): countDetectedSkeleton = " + toString(m_properties->countDetectedSkeleton));
	}

	//clear the old skeletons from pool
	m_skeletonPool.clear();


	//loop over skeletons and add them to pool
	for (auto itSkeleton = m_currFrame->m_skeletons.begin(); itSkeleton != m_currFrame->m_skeletons.end(); itSkeleton++)
	{

		
		m_skeletonPool[skelIdx] = *itSkeleton;


		//loop over all joints and multiply position with offsetmatrix
		for (auto currjoint = m_skeletonPool[skelIdx].m_joints.begin(); currjoint != m_skeletonPool[skelIdx].m_joints.end(); currjoint++)
		{
		
			Vector4f currPos = applyOffset(currjoint->second.getJointPosition());

			currjoint->second.setPosition(currPos);

		}

		skelIdx++;

		m_isDataAvailable = true;

	}

	m_currFrameIdx++;

	m_skeletonPoolLock.unlock();

	//Console::log("mmhPlayer::track(): skeleton count = " + toString(m_skeletonPool.size()));

}

std::string mmhPlayer::getTrackerType()
{
	return "MMH";
}

//std::vector<Vector3f> mmhPlayer::resetOffsets()
//{
//	Vector3f pos = Vector3f(0, 0, 0);
//	Vector3f rot = Vector3f(0, 0, 0);
//	Vector3f scl = Vector3f(1, 1, 1);
//
//	setPositionOffset(pos);
//	setRotationOffset(rot);
//	setScaleOffset(scl);
//
//	std::vector<Vector3f> offsets = { pos, rot, scl };
//
//	return offsets;
//}

void mmhPlayer::controlTime(bool stop)
{
	m_timelineDragging = stop;
}

void mmhPlayer::setCurrentFrame(float newValue)
{

	float frameIdx = (m_frameCount - 1) * newValue / 100.0f;
	m_currFrameIdx = (int)round(frameIdx);

	if (m_isTracking)
	{
		track();
	}
}

float mmhPlayer::getTotalTime()
{

	return m_session.getTotalTime();

}

int mmhPlayer::getCurrFrameIdx()
{
	return m_currFrameIdx;
}

int mmhPlayer::getFrameCount()
{
	return m_frameCount;
}

void mmhPlayer::applyModChange(Joint::JointNames type, Vector3f mod, bool inverted)
{

}


Quaternionf mmhPlayer::convertJointRotation(Quaternionf raw, Joint::JointNames type)
{

	Quaternionf newRot = raw;

	switch (type)
	{
	case Joint::HIPS:
		break;
	case Joint::SPINE:
		break;
	case Joint::CHEST:
		break;
	case Joint::NECK:
		break;
	case Joint::SHOULDER_L:
		break;
	case Joint::ARM_L:
		//newRot = newRot.inverse();
		//newRot *= eulerToQuaternion(Vector3f(180, 0, 180));
		break;
	case Joint::FOREARM_L:
		//newRot *= eulerToQuaternion(Vector3f(0, 0, 180));
		break;
	case Joint::HAND_L:
		break;
	case Joint::SHOULDER_R:
		break;
	case Joint::ARM_R:
		//newRot = newRot.inverse();
		//newRot *= eulerToQuaternion(Vector3f(180, 0, 180));
		break;
	case Joint::FOREARM_R:
		//newRot *= eulerToQuaternion(Vector3f(0, 0, 180));
		break;
	case Joint::HAND_R:
		break;
	case Joint::UPLEG_L:
		//newRot = newRot.inverse();
		break;
	case Joint::LEG_L:
		break;
	case Joint::FOOT_L:
		break;
	case Joint::TOE_L:
		break;
	case Joint::UPLEG_R:
		//newRot = newRot.inverse();
		break;
	case Joint::LEG_R:
		break;
	case Joint::FOOT_R:
		break;
	case Joint::TOE_R:
		break;
	case Joint::HEAD:
		break;
	case Joint::NDEF:
		break;
	default:
		break;
	}








	return newRot;

}