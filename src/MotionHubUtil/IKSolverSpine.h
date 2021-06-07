#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "IKSolver.h"

/*!
 * \class IKSolverSpine
 *
 * \brief Extends IKSolver and implements a IK Solver to solve chains from Hip to Head
 */
class MotionHubUtil_DLL_import_export IKSolverSpine : public IKSolver {

public:

	/*!
	 * Default Constructor
	 */
	IKSolverSpine();

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

protected:

	/*!
	 * Saves default joint positions and rotations
	 */
	virtual void saveDefaultState() override;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() override;

protected:

	Vector3f headLocalPosition;

	Quaternionf headLocalRotation;
};
