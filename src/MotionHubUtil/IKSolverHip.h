#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/IKSolver.h"


class MotionHubUtil_DLL_import_export IKSolverHip : public IKSolver {

protected:

	IKJoint hip;

public:

	IKSolverHip(HierarchicJoint* joint);

	/*!
	 * Initializes the IKSolver
	 */
	virtual void init();

	/*!
	 * Solves current chain to a given position & rotation
	 *
	 * \param position the target position
	 * \param rotation the target rotation
	 */
	virtual void solve(Vector3f position, Quaternionf rotation);

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	void loadDefaultState() override;
};