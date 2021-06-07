#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"
#include "MotionHubUtil/IKJoint.h"

// Todo: Debug entfernen
#include "MotionHubUtil/Console.h"

/*!
 * \class IKSolver
 *
 * \brief Prototype class for IK Solvers - holds virtual methods
 */
class MotionHubUtil_DLL_import_export IKSolver {

public:

	/*!
	 * Default Constructor
	 */
	IKSolver();

	/*!
	 * Default Destructor for Subclasses
	 */
	virtual ~IKSolver();

	/*!
	 * Initializes the IKSolver and stores default values
	 */
	virtual void init() = 0;

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
	virtual void saveDefaultState() = 0;

	/*!
	 * Restores default joint positions and rotations
	 */
	virtual void loadDefaultState() = 0;

protected:

	Vector3f targetPosition;

	Quaternionf targetRotation;
};