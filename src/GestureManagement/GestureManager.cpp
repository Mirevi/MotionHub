#include "GestureManager.h"

// default constructor
GestureManager::GestureManager()
{

	Console::log("GestureManager::GestureManager(): Created gesture manager.");

}

// update all skeleton postures in refferenced skeleton pool
void GestureManager::updateAllSkeletonPostures(std::map<int, Skeleton>* poolSkeletons)
{

	float currHeightHead, heightSkeleton;
	Skeleton::StatePosture previousPosture, currPosture;
	std::string currPostureName;

	// skelton loop
	for (auto itPoolSkeletons = poolSkeletons->begin(); itPoolSkeletons != poolSkeletons->end(); itPoolSkeletons++)
	{

		// get previous posture
		previousPosture = itPoolSkeletons->second.getPosture();
		// get skeleton base height
		heightSkeleton = itPoolSkeletons->second.getHeight();
		// get current height of the head
		currHeightHead = (itPoolSkeletons->second.m_joints[Joint::HEAD].getJointPosition().y() * (-1) + 950) / 1000;


		// detect current posture based on head height
		if (currHeightHead >= heightSkeleton * 1.1f)
		{
			currPosture = Skeleton::StatePosture::JUMP;
			currPostureName = "jumping";
		}
		else if (currHeightHead >= heightSkeleton * 0.9f)
		{
			currPosture = Skeleton::StatePosture::STAND;
			currPostureName = "standing";
		}
		else if (currHeightHead >= heightSkeleton * 0.4f)
		{
			currPosture = Skeleton::StatePosture::CROUCH;
			currPostureName = "crouching";
		}
		else if (currHeightHead < heightSkeleton * 0.3f)
		{
			currPosture = Skeleton::StatePosture::LIE;
			currPostureName = "lie";
		}
		else
		{
			currPosture = Skeleton::StatePosture::UNKNOWN;
			currPostureName = "unkown";
		}

		// update posture if changed
		if (currPosture != previousPosture)
		{

			itPoolSkeletons->second.setPosture(currPosture);

		}
		
	}
}




