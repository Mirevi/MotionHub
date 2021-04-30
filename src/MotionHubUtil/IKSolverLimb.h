#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolver.h"

class MotionHubUtil_DLL_import_export IKSolverLimb : public IKSolver {

public:

	int iterations = 10;
	float tolerance = 0; // 0.001f
	float progressTolerance = 0.0001f;

	float bendToTargetRotationWeight = 0.5f;

	float bendToGoalWeight = 0.9f;

	float middleUntwistWeight = 0.6f;
	float upperUntwistWeight = 0.25f;


	float DebugFloat1 = 0;
	float DebugFloat2 = 0;
	float DebugFloat3 = 0;
	float DebugFloat4 = 0;

protected:

	Vector3f normal;
	Vector3f _normal;

	Quaternionf lowerDefaultRotation;

	Vector3f startPosition;

public:

	IKSolverLimb();

	virtual void init() override;

	virtual void solve(Vector3f position, Quaternionf rotation) override;

	virtual Vector3f getStartPosition();

protected:

	virtual void saveDefaultState();

	virtual void loadDefaultState();

	virtual void solve();

	virtual void constraint();

	virtual void apply();

	virtual void untwist();

	void forwardReach() {
		// Iterate through every position in the list until we reach the start of the chain
		for (int i = joints.size() - 1; i >= 0; i -= 1) {
			// The last bone position should have the same position as the ikTarget
			if (i == joints.size() - 1) {
				joints[i]->setSolvedPosition(targetPosition);
			}
			else {
				Vector3f solvedPosition = solveFABRIK(joints[i]->getSolvedPosition(), joints[i + 1]->getSolvedPosition(), joints[i]->length);
				joints[i]->setSolvedPosition(solvedPosition);
			}
		}
	}

	void backwardReach() {
		// Iterate through every position in the list until we reach the end of the chain
		for (int i = 0; i < joints.size(); i += 1) {
			// The first bone position should have the same position as the startPosition
			if (i == 0) {
				joints[i]->setSolvedPosition(startPosition);
			}
			else {
				Vector3f solvedPosition = solveFABRIK(joints[i]->getSolvedPosition(), joints[i - 1]->getSolvedPosition(), joints[i - 1]->length);
				joints[i]->setSolvedPosition(solvedPosition);
			}
		}
	}

	Vector3f solveFABRIK(Vector3f pos1, Vector3f pos2, float length) {
		return pos2 + (pos1 - pos2).normalized() * length;
	}
};