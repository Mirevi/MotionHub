#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolver.h"

/*!
 * \class IKSolverLeg
 *
 * \brief Extends IKSolver and implements a IK Solver based on the FABRIK algorithm.
 * Solves chains from UpperLeg to Foot / UpperArm to Hand
 */
class MotionHubUtil_DLL_import_export IKSolverLeg : public IKSolver {

public:

	/*!
	 * Default Constructor
	 */
	IKSolverLeg();

	/*!
	 * Constructor with given upper, middle and lower joints
	 * 
	 * \param upper the upper joint
	 * \param middle the middle joint
	 * \param lower the lower joint
	 */
	IKSolverLeg(HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower);

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
	virtual Vector3f getStartPosition();

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
	virtual void solve();

	/*!
	 * Constraints the solution
	 */
	virtual void constraint();

	/*!
	 * Applies the solution to the joints
	 */
	virtual void apply();

	/*!
	 * Untwists the joints based on the target rotation
	 */
	virtual void untwist();

	/*!
	 * The "Forward Reaching" pass of the FABRIK algorithm.
	 * Iterate backward through all joints and clamp their position to be within the range of the joint above it
	 */
	void forwardReach() {

		// Iterate through each joint until from end to start
		for (int i = joints.size() - 1; i >= 0; i -= 1) {

			// The last joint should have the same position as the target position
			if (i == joints.size() - 1) {
				joints[i]->setSolvedPosition(targetPosition);
			}
			else {
				// Solve position to be within the range of the joint above it
				Vector3f jointPosition = joints[i]->getSolvedPosition();
				Vector3f nextJointPosition = joints[i + 1]->getSolvedPosition();
				float jointLength = joints[i]->length;

				Vector3f solvedPosition = solveFABRIK(jointPosition, nextJointPosition, jointLength);

				// Store solved position
				joints[i]->setSolvedPosition(solvedPosition);
			}
		}
	}

	/*!
	 * The "Backward Reaching" pass of the FABRIK algorithm.
	 * Iterate forward through all joints and clamp their position to be within the range of the joint below it
	 */
	void backwardReach() {

		// Iterate through each joint until from start to end
		for (int i = 0; i < joints.size(); i += 1) {

			// The first joint should have the same position as the start position
			if (i == 0) {
				joints[i]->setSolvedPosition(startPosition);
			}
			else {
				// Solve position to be within the range of the joint below it
				Vector3f jointPosition = joints[i]->getSolvedPosition();
				Vector3f prevJointPosition = joints[i - 1]->getSolvedPosition();
				float prevJointLength = joints[i - 1]->length;

				Vector3f solvedPosition = solveFABRIK(jointPosition, prevJointPosition, prevJointLength);

				// Store solved position
				joints[i]->setSolvedPosition(solvedPosition);
			}
		}
	}

	/*!
	 * FABRIK helper function to clamp two positions to be within the range of a desired length
	 * 
	 * \param firstPosition the first position
	 * \param secondPosition the second position
	 * \param length the desired range
	 * \return clamped position
	 */
	Vector3f solveFABRIK(Vector3f firstPosition, Vector3f secondPosition, float length) {
		return secondPosition + (firstPosition - secondPosition).normalized() * length;
	}

public:

	// Break Condition: Max allowed iterations 
	int iterations = 10;

	// Break Condition: How close lower joint should be to the target (in meters) 
	float distanceTolerance = 0; // 0.001f

	// Break Condition: threshold for progress in beteween iterations (in meters) 
	float progressTolerance = 0.0001f;

	// Bend weights for target rotation & goal
	float bendToTargetRotationWeight = 0.5f;
	float bendToGoalWeight = 0.9f;

	// Weights for twisting upper & middle
	float middleUntwistWeight = 0.4f;
	float upperUntwistWeight = 0.25f;


	float DebugFloat1 = 0;
	float DebugFloat2 = 0;
	float DebugFloat3 = 0;
	float DebugFloat4 = 0;

protected:

	IKJoint upperJoint;
	IKJoint middleJoint;
	IKJoint lowerJoint;

	std::vector<IKJoint*> joints;

	Vector3f normal;

	Vector3f defaultNormal;
	Vector3f defaultLocalNormal;

	Quaternionf lowerDefaultRotation;

	Vector3f startPosition;
};