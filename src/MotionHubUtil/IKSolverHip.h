#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/IKSolver.h"


class MotionHubUtil_DLL_import_export IKSolverHip : public IKSolver {

protected:

	IKJoint hip;

public:

	IKSolverHip(HierarchicJoint* joint);

	virtual void init();

	virtual void solve(Vector3f position, Quaternionf rotation);
};