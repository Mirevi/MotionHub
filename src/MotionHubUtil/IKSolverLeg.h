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
	 * Stores a reference to toe joint
	 */
	//void seToe(HierarchicJoint* joint);

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

	void solve4(Vector4f position, Quaternionf rotation) {

		rotation = Quaternionf(rotation.y(), rotation.z(), -rotation.w(), -rotation.x());
		solve(Vector3f(position.x(), position.y(), position.z()), rotation);
	}

	virtual void hint(Vector3f position, Quaternionf rotation);

	void hint4(Vector4f position, Quaternionf rotation) {

		rotation = Quaternionf(rotation.y(), rotation.z(), -rotation.w(), -rotation.x());
		hint(Vector3f(position.x(), position.y(), position.z()), rotation);
	}

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
	int iterations = 30; // 10

	// Break Condition: How close lower joint should be to the target (in meters) 
	float distanceTolerance = 0.0001f; // 0.001f

	// Break Condition: threshold for progress in beteween iterations (in meters) 
	float progressTolerance = 0.000001f; // 0.00001f

	// Delta for rotating upper to target rotation
	float upperNormalToTargetDelta = 0.5f;

	// Delta for rotating middle to target rotation
	float middleNormalToTargetDelta = 0.5f;

protected:

	IKJoint upperJoint;
	IKJoint middleJoint;
	IKJoint lowerJoint;

	/*
	HierarchicJoint* toe = nullptr;
	float toeCalibratedY = 0;
	float toeLength = 0;

	Vector3f toeDefaultPosition;
	Quaternionf toeDefaultRotation;
	*/

	std::vector<IKJoint*> joints;

	bool isLeft;
	bool isLeg;

	Vector3f normal;
	Vector3f lastNormal;
	Vector3f lastMiddleNormal;

	Vector3f defaultLocalNormal;

	Quaternionf invMiddleDefaultRotation;
	Quaternionf invLowerDefaultRotation;

	Vector3f startPosition;

	float length;

	Vector3f hintPosition;
	Quaternionf hintRotation;

	Vector3f targetForward;

	Vector3f lastUpperToTarget;
};