#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"
#include <MotionHubUtil/Skeleton.h>

/*!
 * \class HierarchicSkeleton
 *
 * \brief Wrapper for multiple HierarchicJoints with fields to access them directly
 */
class MotionHubUtil_DLL_import_export HierarchicSkeleton {

public:

	/*!
	 * Initializes the HierarchicSkeleton with default values
	 */
	void init();

	void reset();

	/*!
	 * Inserts all Joints from HierarchicSkeleton into given Skeleton
	 * 
	 * \param currSkeleton pointer to Skeleton
	 */
	void insert(Skeleton* currSkeleton);

	void setScale(Vector3f scale);

	Vector3f getScale();

protected:

	/*!
	 * Adds a Joint to the Joint collection vector
	 * 
	 * \param joint the Joint to add
	 * \param jointName the Joint::JointName
	 */
	void addJoint(HierarchicJoint* joint, Joint::JointNames jointName);

public:

	// joint collection
	std::vector<HierarchicJoint*> joints;

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
};