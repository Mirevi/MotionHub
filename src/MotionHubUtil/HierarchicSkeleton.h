#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "HierarchicJoint.h"
#include "Skeleton.h"

/*!
 * \class HierarchicSkeleton
 *
 * \brief Wrapper for multiple HierarchicJoints with fields to access them directly
 */
class MotionHubUtil_DLL_import_export HierarchicSkeleton {

public:

	HierarchicSkeleton(int sid = 1);

	/*!
	 * Initializes the HierarchicSkeleton with default values
	 */
	void init();

	void reset();

	void invalidateGlobals();

	/*!
	 * Inserts all Joints from HierarchicSkeleton into given Skeleton
	 * 
	 * \param currSkeleton pointer to Skeleton
	 */
	void insert(Skeleton* currSkeleton);

	void setScale(Vector3f scale);

	Vector3f getScale();

	HierarchicJoint* getJoint(Joint::JointNames jointName);

protected:

	/*!
	 * Adds a Joint to the Joint collection vector
	 * 
	 * \param joint the Joint to add
	 * \param jointName the Joint::JointName
	 */
	void addJoint(HierarchicJoint* joint, Joint::JointNames jointName);

public:

	Vector3f scale;

	// joint collection
	std::vector<HierarchicJoint*> joints;

	HierarchicJoint hips;

	HierarchicJoint leftUpLeg;
	HierarchicJoint leftLeg;
	HierarchicJoint leftFoot;
	HierarchicJoint leftToe;
	//HierarchicJoint leftToeEnd;

	HierarchicJoint rightUpLeg;
	HierarchicJoint rightLeg;
	HierarchicJoint rightFoot;
	HierarchicJoint rightToe;
	//HierarchicJoint rightToeEnd;

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

	Skeleton skeleton;
};