#include "GestureManager.h"

GestureManager::GestureManager()
{



}

void GestureManager::updateAllSkeletonPostures(std::map<int, Skeleton>* poolSkeletons)
{

	float heightHead;
	Skeleton::StatePosture previousPosture, currPosture;
	std::string currGestureName;

	for (auto itPoolSkeletons = poolSkeletons->begin(); itPoolSkeletons != poolSkeletons->end(); itPoolSkeletons++)
	{

		previousPosture = itPoolSkeletons->second.getPosture();
		heightHead = (itPoolSkeletons->second.m_joints[Joint::HEAD].getJointPosition().m_xyz.y * (-1) + 950) / 1000;
		
		Console::log("GestureManager::updateAllSkeletonPostures(): Skeleton with id = " + std::to_string(itPoolSkeletons->second.getSid()) + " height head = " + std::to_string(heightHead) + ".");

		if (heightHead >= 1.3f)
		{
			currPosture = Skeleton::StatePosture::STAND;
			currGestureName = "standing";
		}
		else if (heightHead >= 0.3f)
		{
			currPosture = Skeleton::StatePosture::CROUCH;
			currGestureName = "crouching";
		}
		else if (heightHead < 0.3f)
		{
			currPosture = Skeleton::StatePosture::LIE;
			currGestureName = "lie";
		}
		else
		{
			currPosture = Skeleton::StatePosture::UNKNOWN;
			currGestureName = "unkown";
		}

		itPoolSkeletons->second.setPosture(currPosture);

		Console::log("GestureManager::updateAllSkeletonPostures(): Skeleton with id = " + std::to_string(itPoolSkeletons->second.getSid()) + "| previous posture = " + std::to_string(previousPosture) + " | current posture = " + std::to_string(currPosture) + ".");

		if (currPosture != previousPosture)
		{
			//itPoolSkeletons->second.setPosture(currPosture);
			Console::log("GestureManager::updateAllSkeletonPostures(): Updated skeleton with id = " + std::to_string(itPoolSkeletons->second.getSid()) + " posture: " + currGestureName + ".");
		}

	}
}