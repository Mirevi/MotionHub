#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"

// Todo: Debug entfernen
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/IKJoint.h"

class MotionHubUtil_DLL_import_export IKSolver {

public:


protected:

	Vector3f targetPosition;
	Quaternionf targetRotation;

public:

	IKSolver();

	// Allow Subclasses to  be destructed in polymorphism
	virtual ~IKSolver();

	virtual void init() = 0;

	virtual void solve(Vector3f position, Quaternionf rotation);

protected:

};