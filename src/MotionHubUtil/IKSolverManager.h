#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicSkeleton.h"
#include "MotionHubUtil/IKSolverHip.h"
#include "MotionHubUtil/IKSolverSpine.h"
#include "MotionHubUtil/IKSolverLeg.h"
#include "MotionHubUtil/IKSolverArm.h"

class MotionHubUtil_DLL_import_export IKSolverManager {

public:

	IKSolverManager();

	~IKSolverManager();

	void initSkeleton(Skeleton otherSkeleton, int sid);

	void initSkeleton(int sid);

	void initIKSolvers(HierarchicSkeleton* hierarchicSkeleton);

	void initIKSolvers();

	void enableDirectMode();

	void refreshIKSolvers(bool overrideDefault);

	IKSolverHip* hipSolver() {
		return ikSolverHip;
	}

	IKSolverSpine* spineSolver() {
		return ikSolverSpine;
	}

	IKSolverLeg* leftLegSolver() {
		return ikSolverLeftLeg;
	}

	IKSolverLeg* rightLegSolver() {
		return ikSolverRightLeg;
	}

	IKSolverArm* leftArmSolver() {
		return ikSolverLeftArm;
	}

	IKSolverArm* rightArmSolver() {
		return ikSolverRightArm;
	}

	bool isInitialized() {
		return initialized;
	}

	Skeleton* getSolvedSkeleton() {

		// Copy ik data to skeleton
		hierarchicSkeleton->insert();
		skeleton->m_joints = hierarchicSkeleton->skeleton.m_joints;

		return skeleton;
	}

	HierarchicSkeleton* hierarchicSkeleton = nullptr;

private:

	
	Skeleton* skeleton = nullptr;

	// IKSolvers
	IKSolverHip* ikSolverHip = nullptr;
	IKSolverSpine* ikSolverSpine = nullptr;
	IKSolverLeg* ikSolverLeftLeg = nullptr;
	IKSolverLeg* ikSolverRightLeg = nullptr;
	IKSolverArm* ikSolverLeftArm = nullptr;
	IKSolverArm* ikSolverRightArm = nullptr;

	bool initialized = false;
};