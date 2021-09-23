#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolverLeg.h"

/*!
 * \class IKSolverArm
 *
 * \brief Extends IKSolverLeg and implements a IK Solver based on the FABRIK algorithm.
 * Solves chains from Shoulder to Hand
 */
class MotionHubUtil_DLL_import_export IKSolverArm : public IKSolverLeg {

public:

	/*!
	 * Default Constructor
	 */
	IKSolverArm();

	/*!
	 * Constructor with given upper, middle and lower joints
	 *
	 * \param upper the upper joint
	 * \param middle the middle joint
	 * \param lower the lower joint
	 */
	IKSolverArm(HierarchicJoint* shoulder, HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower);

	/*!
	 * Stores a reference to chest joint
	 */
	void setChest(HierarchicJoint* joint);

	/*!
	 * Initializes the IKSolver and stores default values
	 */
	virtual void init() override;

	virtual void refresh(bool overrideDefault = false) override;

	/*!
	 * Solves current chain to a given position & rotation
	 *
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation) override;

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	virtual void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() override;

	virtual void updateNormal() override;

	/*!
	 * Solves the chain with the FABRIK algorithm
	 */
	virtual void solve() override;

	/*!
	 * Constraints the solution
	 */
	virtual void constraint() override;

	/*!
	 * Applies the solution to the joints
	 */
	virtual void apply() override;

	/*!
	 * Untwists the joints based on the target rotation
	 */
	virtual void untwist() override;

public:
	bool solveShoulder = true;

protected:

	IKJoint shoulderJoint;

	HierarchicJoint* chest;

	Quaternionf invShoulderDefaultRotation;

	Vector3f lastShoulderToHint;

	Vector3f handToPinky;

	float shoulderBackYAngle = 35.0f; // Back
	float shoulderForwardYAngle = 45.0f; // Forward
	float shoulderUpZAngle = 50.0f; // Up
	float shoulderDownMaxAngle = 35.0f; // Down

	float defaultShoulderHandDistance;
	float shoulderHandMaxAngle = 25.0f;
};