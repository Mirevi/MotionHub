#include "GestureManager.h"

GestureManager::GestureManager()
{



}

void GestureManager::updateAllSkeletonPostures(std::map<int, Skeleton*>* poolSkeletons)
{

	float currHeightHead, heightSkeleton;
	Skeleton::StatePosture previousPosture, currPosture;
	std::string currGestureName;



	for (auto itPoolSkeletons = poolSkeletons->begin(); itPoolSkeletons != poolSkeletons->end(); itPoolSkeletons++)
	{

		if (itPoolSkeletons->second != nullptr)
		{

			previousPosture = itPoolSkeletons->second->getPosture();
			heightSkeleton = itPoolSkeletons->second->getHeight();
			currHeightHead = (itPoolSkeletons->second->m_joints[Joint::HEAD].getJointPosition().m_xyz.y * (-1) + 950) / 1000;

			//Console::log("GestureManager::updateAllSkeletonPostures(): Skeleton with id = " + std::to_string(itPoolSkeletons->second->getSid()) + " height head = " + std::to_string(heightHead) + ".");

			if (currHeightHead >= heightSkeleton * 1.1f)
			{
				currPosture = Skeleton::StatePosture::JUMP;
				currGestureName = "jumping";
			}
			else if (currHeightHead >= heightSkeleton * 0.9f)
			{
				currPosture = Skeleton::StatePosture::STAND;
				currGestureName = "standing";
			}
			else if (currHeightHead >= heightSkeleton * 0.4f)
			{
				currPosture = Skeleton::StatePosture::CROUCH;
				currGestureName = "crouching";
			}
			else if (currHeightHead < heightSkeleton * 0.3f)
			{
				currPosture = Skeleton::StatePosture::LIE;
				currGestureName = "lie";
			}
			else
			{
				currPosture = Skeleton::StatePosture::UNKNOWN;
				currGestureName = "unkown";
			}

			if (currPosture != previousPosture)
			{
				itPoolSkeletons->second->setPosture(currPosture);
				//Console::log("GestureManager::updateAllSkeletonPostures(): Skeleton with id = " + std::to_string(itPoolSkeletons->second->getSid()) + " | previous posture = " + std::to_string(previousPosture) + " | current posture = " + std::to_string(currPosture) + ".");
				Console::log("GestureManager::updateAllSkeletonPostures(): Updated skeleton with id = " + std::to_string(itPoolSkeletons->second->getSid()) + " posture: " + currGestureName + ".");
			}
		}
	}
}