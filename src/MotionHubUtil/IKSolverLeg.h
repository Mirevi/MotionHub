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

	virtual void refresh(bool overrideDefault = false);

	/*!
	 * Solves current chain to a given position & rotation
	 *
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation) override;

	virtual void hint(Vector3f position, Quaternionf rotation);

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	virtual void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() override;


	virtual void updateNormal();

	/*!
	 * Solves the chain with the FABRIK algorithm
	 */
	virtual void solve();

	/*!
	 * Constraints the solution
	 */
	virtual void constraint();

	virtual void constraintSolvePosition(Vector3f startPosition);

	/*!
	 * Applies the solution to the joints
	 */
	virtual void apply();

	/*!
	 * The "Forward Reaching" pass of the FABRIK algorithm.
	 * Iterate backward through all joints and clamp their position to be within the range of the joint above it
	 */
	void forwardReach() {

		// Iterate through each joint until from end to start
		for (int i = joints.size() - 1; i >= 0; i -= 1) {

			// The last joint should have the same position as the target position
			if (i == joints.size() - 1) {
				joints[i]->setSolvedPosition(solvePosition);
			}
			else {
				// Solve position to be within the range of the joint above it
				Vector3f jointPosition = joints[i]->getSolvedPosition();
				Vector3f nextJointPosition = joints[i + 1]->getSolvedPosition();
				float jointLength = joints[i]->length;

				Vector3f solvedPosition = IKSolver::solveFABRIK(jointPosition, nextJointPosition, jointLength);

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

				Vector3f solvedPosition = IKSolver::solveFABRIK(jointPosition, prevJointPosition, prevJointLength);

				// Store solved position
				joints[i]->setSolvedPosition(solvedPosition);
			}
		}
	}

public:

	// Break Condition: Max allowed iterations 
	int iterations = 20; // 10

	// Break Condition: How close lower joint should be to the target (in meters) 
	float distanceTolerance = 0.0001f; // 0.001f

	// Break Condition: threshold for progress in beteween iterations (in meters) 
	float progressTolerance = 0.000001f; // 0.00001f

	// Bend weights for target rotation & hint
	float slerpTargetRotationDelta = 0.5f;
	float slerpHintDelta = 0.9f;

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

	bool isLeft;

	Vector3f normal;
	Vector3f lastNormal;
	Vector3f lastMiddleNormal;

	Vector3f defaultLocalNormal;

	Quaternionf invUpperDefaultRotation;
	Quaternionf invMiddleDefaultRotation;
	Quaternionf invLowerDefaultRotation;

	Vector3f startPosition;

	float length;

	Vector3f hintPosition;
	Quaternionf hintRotation;
	bool hasHint = false;

	Vector3f middleToUpper;
};