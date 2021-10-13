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
 * FABRIK Algorithm source: https://unitylist.com/p/yfx/Easy-IK
 * CCD Algorithm source: https://github.com/zalo/MathUtilities
 */
class MotionHubUtil_DLL_import_export IKSolver {
public:

	Vector3f DebugPos1 = Vector3f::Zero();
	Vector3f DebugPos2 = Vector3f::Zero();
	Vector3f DebugPos3 = Vector3f::Zero();
	Vector3f DebugPos4 = Vector3f::Zero();

	bool DebugBool1 = false;
	bool DebugBool2 = false;
	bool DebugBool3 = false;

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

	void enableCalibrationMode() {
		isCalibrating = true;
	}

	void disableCalibrationMode() {
		isCalibrating = false;
	}

	/*!
	 * FABRIK helper function to clamp two positions to be within the range of a desired length
	 *
	 * \param firstPosition the first position
	 * \param secondPosition the second position
	 * \param length the desired range
	 * \return clamped position
	 */
	static Vector3f solveFABRIK(const Vector3f& firstPosition, const Vector3f& secondPosition, const float length) {
		return secondPosition + (firstPosition - secondPosition).normalized() * length;
	}

	/*!
	 * CCD helper function to create rotations between two vectors: last - current & target - current
	 *
	 * \param firstPosition the current position
	 * \param secondPosition the last position
	 * \param targetPosition the target position
	 * \return rotation between two vectors: last - current & target - current
	 */
	static Quaternionf solveCCD(const Vector3f& currentPosition, const Vector3f& lastPosition, const Vector3f& targetPosition) {

		// Store vectors from joint to head & target
		Vector3f jointToLast = lastPosition - currentPosition;
		Vector3f jointToTarget = targetPosition - currentPosition;

		// Return rotation from current rotation towards target 
		return fromToRotation(jointToLast, jointToTarget);
	}


	static Vector3f calcInverseKinematic(Vector3f start, Vector3f end, float length0, float length1, Vector3f up) {
		Vector3f startToEnd = (end - start);
		
		float length = startToEnd.norm();

		float ld = (length1 * length1 - length0 * length0 + length * length) / (2 * length);

		float h = length1 * length1 - ld * ld;
		h = h >= 0 ? sqrtf(h) : 0;

		Vector3f forward = startToEnd.normalized();
		Vector3f right = up.cross(forward).normalized();

		up = forward.cross(right).normalized();

		//if (basti2) {
		// ! GUT mit normal
		//return (end - ld * forward + h * up) - right;
		//}
		return end - ld * forward + h * up;
	}

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

	Vector3f solvePosition;

	Quaternionf targetRotation;

	bool isCalibrating = false;
	bool hasHint = false;
};