#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"

class MotionHubUtil_DLL_import_export IKSolver {

public:


protected:

	Vector3f targetPosition;
	Quaternionf targetRotation;

public:

	IKSolver();

	// Allow Subclasses to  be destructed in polymorphism
	virtual ~IKSolver();

	virtual void initialize();

	virtual void solve(Vector3f position, Quaternionf rotation);

protected:

};