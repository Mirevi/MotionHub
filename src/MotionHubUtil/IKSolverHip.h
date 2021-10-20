#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/IKSolver.h"


class MotionHubUtil_DLL_import_export IKSolverHip : public IKSolver {

protected:

	HierarchicJoint* hip;

	HierarchicJoint* leftUpperLeg;
	HierarchicJoint* rightUpperLeg;

	HierarchicJoint* leftFoot;
	HierarchicJoint* rightFoot;

	Vector3f leftFootPosition;
	Vector3f rightFootPosition;

	float leftLegLength;
	float rightLegLength;

	float leftLegReach;
	float rightLegReach;

public:

	IKSolverHip(HierarchicJoint* joint);


	/*!
	 * Stores a reference to left leg joints
	 */
	void setLeftLeg(HierarchicJoint* upper, HierarchicJoint* foot);

	/*!
	 * Stores a reference to right leg joints
	 */
	void setRightLeg(HierarchicJoint* upper, HierarchicJoint* foot);

	/*!
	 * Initializes the IKSolver
	 */
	virtual void init();

	virtual void refresh(bool overrideDefaults = false);

	/*!
	 * Solves current chain to a given position & rotation
	 *
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation);

	void solve4(Vector4f position, Quaternionf rotation) {

		rotation = Quaternionf(rotation.y(), rotation.z(), -rotation.w(), -rotation.x());
		solve(Vector3f(position.x(), position.y(), position.z()), rotation);
	}

	virtual void hint(Vector3f leftFoot, Vector3f rightFoot);

	void hint4(Vector4f leftFoot, Vector4f rightFoot) {
		hint(Vector3f(leftFoot.x(), leftFoot.y(), leftFoot.z()), Vector3f(rightFoot.x(), rightFoot.y(), rightFoot.z()));
	}

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	void loadDefaultState() override;
};