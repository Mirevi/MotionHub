#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"
#include <MotionHubUtil/Skeleton.h>

class MotionHubUtil_DLL_import_export HierarchicSkeleton {

public:

	HierarchicJoint hips;

	HierarchicJoint leftUpLeg;
	HierarchicJoint leftLeg;
	HierarchicJoint leftFoot;
	HierarchicJoint leftToe;

	HierarchicJoint rightUpLeg;
	HierarchicJoint rightLeg;
	HierarchicJoint rightFoot;
	HierarchicJoint rightToe;

	HierarchicJoint spine;
	HierarchicJoint chest;

	HierarchicJoint leftShoulder;
	HierarchicJoint leftArm;
	HierarchicJoint leftForeArm;
	HierarchicJoint leftHand;

	HierarchicJoint neck;
	HierarchicJoint head;

	HierarchicJoint rightShoulder;
	HierarchicJoint rightArm;
	HierarchicJoint rightForeArm;
	HierarchicJoint rightHand;

	std::vector<HierarchicJoint*> joints;

public:

	void init();

	void insert(Skeleton* currSkeleton);

protected:

	void addJoint(HierarchicJoint* joint, Joint::JointNames jointName);
};