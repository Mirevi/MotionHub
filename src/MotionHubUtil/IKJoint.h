#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"

class MotionHubUtil_DLL_import_export IKJoint {

public:
	// TODO: IKJoint von HierarchicJoint erben lassen?
	HierarchicJoint* joint;

	// TODO: Zusätzlich length speichern? -> sqrt vermeiden
	float sqrMagnitude = 0;
	float length = 0;

protected:

	Vector3f solvedPosition;

public:

	/*!
	 * Default constructor. Sets joint to null
	 */
	IKJoint() {
		joint = nullptr;
	}

	/*!
	 * Constructor with given joint
	 */
	IKJoint(HierarchicJoint* hierarchicJoint) {
		joint = hierarchicJoint;
	}


	void storeSolvedPosition() {
		setSolvedPosition(joint->getGlobalPosition());
	}

	void setSolvedPosition(Vector3f position) {
		solvedPosition = position;
	}

	Vector3f getSolvedPosition() {
		return solvedPosition;
	}
};