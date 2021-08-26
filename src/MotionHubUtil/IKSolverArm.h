#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolverLeg.h"

/*!
 * \class IKSolverArm
 *
 * \brief Extends IKSolverLeg and implements a IK Solver based on the FABRIK algorithm.
 * Solves chains from Shoulder to Hand
 * FABRIK Algorithm taken from https://unitylist.com/p/yfx/Easy-IK
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

	/*!
	 * Solves current chain to a given position & rotation
	 *
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation) override;

	/*!
	 * Returns the current start position of the chain
	 *
	 * \return the current position of the start joint
	 */
	virtual Vector3f getStartPosition() override;

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	virtual void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() override;

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
	

protected:

	IKJoint shoulderJoint;

	HierarchicJoint* chest;

	bool solveShoulder = false;
};