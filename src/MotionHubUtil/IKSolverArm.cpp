#include "IKSolverArm.h"

IKSolverArm::IKSolverArm() {

}

IKSolverArm::IKSolverArm(HierarchicJoint* shoulder, HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower)
// Base Constructor
	: IKSolverLeg(upper, middle, lower) {

	// Create IKJoint object from shoulder
	shoulderJoint = IKJoint(shoulder);
}

void IKSolverArm::setChest(HierarchicJoint* joint) {

	chest = joint;
}

void IKSolverArm::init() {

	IKSolverLeg::init();

	// Initiate joint
	Vector3f shoulderToUpper = upperJoint.getPosition() - shoulderJoint.getPosition();
	shoulderJoint.init(shoulderToUpper, normal);

	handToPinky = Vector3f(0, 0, -1);

	handToJoint = Vector3f(1, 0, 0);
	if (isLeft) {
		handToJoint = Vector3f(-1, 0, 0);
	}

	if (isLeft) {
		float tempAngle = shoulderUpZAngle;
		shoulderUpZAngle = shoulderDownZAngle;
		shoulderDownZAngle = tempAngle;
	}

	refresh(true);
}

void IKSolverArm::refresh(bool overrideDefault) {

	IKSolverLeg::refresh(overrideDefault);

	if (overrideDefault) {
		shoulderJoint.saveDefaultState();
	}

	Vector3f shoulderToUpper = upperJoint.getPosition() - shoulderJoint.getPosition();
	shoulderJoint.setLength(shoulderToUpper);

	// Save shoulder to hand distance
	defaultShoulderHandDistance = (lowerJoint.getPosition() - shoulderJoint.getPosition()).norm();
}

void IKSolverArm::saveDefaultState() {

	// Save default state for shoulder
	shoulderJoint.saveDefaultState();

	invShoulderDefaultRotation = shoulderJoint.getRotation().inverse();

	// Call save on base class
	IKSolverLeg::saveDefaultState();
}

void IKSolverArm::loadDefaultState() {

	// Save default state for shoulder
	//shoulderJoint.loadDefaultState();

	// Call load on base class
	IKSolverLeg::loadDefaultState();
}

void IKSolverArm::updateNormal() {

	targetForward = targetRotation * -handToJoint;

	//upperNormalToTargetDelta = 0.5f;

	// Use leg normal calculation
	IKSolverLeg::updateNormal();

	// Exit if in calibration mode or hint exists
	if (isCalibrating || hasHint) {
		return;
	}

	// Get normal from current rotation
	Quaternionf upperRotation = upperJoint.getRotation();
	Vector3f upperNormal = upperRotation * defaultLocalNormal;

	// Create axis from upper joint to target
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f axis = (solvePosition - upperPosition).normalized();

	// Rotate upperNormal towards axis
	Vector3f upperToChild = (middleJoint.getPosition() - upperPosition).normalized();
	upperNormal = fromToRotation(upperToChild, axis) * upperNormal;

	// Slerp normal betweenUpperLower upper & calculated leg normal
	normal = slerp(upperNormal.normalized(), normal.normalized(), 0.6f).normalized();
}

void IKSolverArm::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolverLeg::solve(position, rotation);
}

void IKSolverArm::solve() {

	shoulderJoint.loadDefaultState();

	// TODO: Ueberarbeiten
	if (!isCalibrating) {

		// Read shoulder positon & relative shoulder rotation
		Vector3f shoulderPosition = shoulderJoint.getPosition();
		Quaternionf localShoulderRotation = shoulderJoint.getRotation() * invShoulderDefaultRotation;

		// Calculate vector from shoulder to current elbow 
		Vector3f toMiddle = middleJoint.getPosition() - shoulderPosition;

		Vector3f shoulderSolvePosition = targetPosition;
		if (distance(shoulderPosition, targetPosition) >= length + shoulderJoint.length) {
			shoulderSolvePosition = shoulderPosition + (targetPosition - shoulderPosition).normalized() * (length + shoulderJoint.length);
		}

		// Estimate elbow position & calculate vector fom shoulder to it
		Vector3f helpAxis = (shoulderSolvePosition - shoulderPosition).cross(normal);
		float shoulderDelta = shoulderJoint.length * 0.6f;

		// get or calculate hint position
		Vector3f middleHint = hintPosition;
		if (!hasHint) {
			middleHint = calcInverseKinematic(shoulderPosition, shoulderSolvePosition, upperJoint.length + shoulderDelta, middleJoint.length + shoulderDelta, helpAxis);
		}

		Vector3f toHint = middleHint - shoulderPosition;

		Vector3f toNewMiddle = toHint;
		if (!hasHint) {
			toNewMiddle = slerp(lastShoulderToHint, toHint, 0.5f);
		}

		// Calculate rotation from current elbow to new elbow position 
		Quaternionf solvedRotation = fromToRotation(toMiddle, toNewMiddle);

		lastShoulderToHint = toHint;

		// Extract local shoulder y- & z-axis
		//Vector3f xAxis = localShoulderRotation * Vector3f(1, 0, 0);
		Vector3f yAxis = localShoulderRotation * Vector3f(0, 1, 0);
		Vector3f zAxis = localShoulderRotation * Vector3f(0, 0, 1);

		// Decompose y rotation & calculate y angle
		Quaternionf yRotation = decomposeTwist(solvedRotation, yAxis);
		float yAngle = signedAngle(zAxis, yRotation * zAxis, yAxis);

		// Decompose z rotation & calculate z angle
		Quaternionf zRotation = decomposeTwist(solvedRotation, zAxis);
		float zAngle = signedAngle(yAxis, zRotation * yAxis, zAxis);

		// Find out if elbow is infront oder behind shoulder
		bool isForward = false;

		// Left: Is y angle negative? -> position is infront of shoulder
		if (isLeft && yAngle < 0) {
			isForward = true;
		}

		// Right: Is y angle positive? -> position is infront of shoulder
		if (!isLeft && yAngle >= 0) {
			isForward = true;
		}

		// Add extra allowed angle by shoulder-target distance
		float handYAngle = mapClamp(shoulderToTargetOnUp.norm(), 0, defaultShoulderHandDistance, 0, shoulderHandMaxAngle);

		float clampAngle = 80;

		// Clamp y Angle by config
		float yDeltaAngle;
		if (isForward) {
			if (yAngle >= 0) {
				yDeltaAngle = mapClamp(yAngle, 0, clampAngle, 0, shoulderForwardYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -clampAngle, 0, -shoulderForwardYAngle - handYAngle, 0);
			}
		}
		else {
			if (yAngle >= 0) {
				yDeltaAngle = mapClamp(yAngle, 0, clampAngle, 0, shoulderBackYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -clampAngle, 0, -shoulderBackYAngle - handYAngle, 0);
			}
		}

		// Clamp y rotation by shoulder-elbow angle & shoulder-hand distance
		yRotation = rotateTowards(Quaternionf::Identity(), yRotation, abs(yDeltaAngle));

		// Find out if elbow is above oder under shoulder
		bool isUp = false;

		// Left: Is z angle negative? -> position is above of shoulder
		if (isLeft && zAngle < 0) {
			isUp = true;
		}

		// Left: Is z angle negative? -> position is above of shoulder
		if (!isLeft && zAngle < 0) {
			isUp = true;
		}

		// Add extra allowed angle by shoulder-target distance
		float handZAngle = mapClamp(shoulderToTargetOnForward.norm(), 0, defaultShoulderHandDistance, 0, shoulderHandMaxAngle);

		// Clamp z Angle by config
		float zDeltaAngle;
		if (isUp) {
			if (zAngle >= 0) {
				zDeltaAngle = mapClamp(zAngle, 0, clampAngle, 0, shoulderUpZAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -clampAngle, 0, -shoulderUpZAngle - handZAngle, 0);
			}
		}
		else {
			if (zAngle >= 0) {
				zDeltaAngle = mapClamp(zAngle, 0, clampAngle, 0, shoulderDownZAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -clampAngle, 0, -shoulderDownZAngle - handZAngle, 0);
			}
		}

		// Clamp z rotation by shoulder-elbow angle & shoulder-hand distance
		zRotation = rotateTowards(Quaternionf::Identity(), zRotation, abs(zDeltaAngle));

		// Get rotation from y- & z-rotation
		Quaternionf rotation = (yRotation * zRotation);

		Quaternionf newShoulderRotation = rotation * shoulderJoint.getRotation();

		// Set shoulder rotation
		if (!hasHint) {

			//shoulderJoint.setRotation(lastShoulderRotation.slerp(0.5f, newShoulderRotation));
			shoulderJoint.setRotation(newShoulderRotation);
		}
		else {

			shoulderJoint.setRotation(newShoulderRotation);

			// Create vector from hint position to sovled upper
			Vector3f upperPosition = upperJoint.getPosition();
			Vector3f newUp = hintPosition + (upperPosition - hintPosition).normalized() * upperJoint.length;
			DebugPos3 = newUp;

			// create & apply new rotation
			Vector3f shoulderToUp = upperPosition - shoulderPosition;
			Vector3f shoulderToNewUp = newUp - shoulderPosition;

			rotation = fromToRotation(shoulderToUp, shoulderToNewUp);
			newShoulderRotation = rotation * shoulderJoint.getRotation();
			shoulderJoint.setRotation(newShoulderRotation);
		}

		// update solved positions for chain
		for (IKJoint* joint : joints) {
			joint->saveSolvedPosition();
		}
	}

	IKSolverLeg::solve();
}

void IKSolverArm::constraint() {

	IKSolverLeg::constraint();

	if (isCalibrating || hasHint) {
		return;
	}

	// Store solved positions of upper, middle & lower position
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f middlePosition = middleJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Get the rotation limb axis from upper to lower joint
	Vector3f limbAxis = (lowerPosition - upperPosition).normalized();

	// Store current direction: upper to middle
	Vector3f currentDirection = (middlePosition - upperPosition).normalized();

	// Move current direction outside
	currentDirection += (solvePosition - upperPosition).cross(normal).normalized() * 0.1f;

	// Create direction from current direction 
	Vector3f bendDirection = createBendDirection(currentDirection, limbAxis);

	// project both directions on limb axis plane
	currentDirection = projectOnPlane(currentDirection.normalized(), limbAxis);
	bendDirection = projectOnPlane(bendDirection.normalized(), limbAxis);

	//DebugPos1 = upperPosition + limbAxis;
	//DebugPos2 = closestPoint;
	//DebugPos3 = closestPoint + hintOffset;
	//DebugPos4 = upperPosition + bendDirection;

	// Calculate angle difference on limb axis & create rotation on axis
	float angle = signedAngle(currentDirection, bendDirection, limbAxis);
	Quaternionf rotation = angleAxis(angle, limbAxis);

	// Rotate the middle bone using the angle
	Vector3f newMiddlePosition = rotation * (middlePosition - upperPosition) + upperPosition;
	middleJoint.setSolvedPosition(newMiddlePosition);
}

Vector3f IKSolverArm::createBendDirection(const Vector3f& currentDirection, const Vector3f& limbAxis) {

	// Store solved positions of upper solved position
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Get the rotation limb axis from upper to lower joint
	// Store current direction
	Vector3f bendDirection = currentDirection;

	// Calculate upper candidate for middle with bend direction
	Vector3f upperCandidate = upperPosition + bendDirection.normalized() * upperJoint.length;

	// Rotate bend direction towards chest down
	Vector3f chestUp = chest->getUp();
	Vector3f chestDown = -chestUp;
	Quaternionf towardsChestDown = fromToRotation(bendDirection, chestDown);
	bendDirection = (Quaternionf::Identity().slerp(0.8f, towardsChestDown) * bendDirection);

	// Calculate lower candidate for middle with rotated bend direction
	Vector3f lowerCandidate = upperPosition + bendDirection.normalized() * upperJoint.length;

	// Move upper & lower candidate up by half middle length
	float halfMiddleJointLength = middleJoint.length * 0.5f;
	upperCandidate += Vector3f(0, halfMiddleJointLength, 0);
	lowerCandidate += Vector3f(0, -halfMiddleJointLength, 0);

	// Calculate angle between target & chest up
	Vector3f chestForward = chest->getForward();
	float angleUp = angleBetween(projectOnPlane(limbAxis, chestForward), chestUp);

	// Is hand position above shoulder?
	if (angleUp <= 120.0f) {

		// move upper candidate towards down
		float mappedAngleUp = mapClamp(angleUp, 120, 0, 0, 0.9f);
		upperCandidate += Vector3f(0, -halfMiddleJointLength * mappedAngleUp, 0);
	}

	// Create position & vector between upper & lower candidate
	Vector3f centerUpperLower = lerp(upperCandidate, lowerCandidate, 0.5f);
	Vector3f toCenterUpperLower = (centerUpperLower - solvePosition).normalized();

	// Create Hand back vector
	Vector3f toHandJoint = targetRotation * (handToJoint * 0.5f);
	Vector3f toHandPinky = targetRotation * (handToPinky * 0.2f);

	// Create point by slerping between center upper/lower candidat & hand back vectors
	float slerpPointBetweenBack = 0.4f;
	Vector3f point = solvePosition + slerp(toCenterUpperLower, (toHandJoint + toHandPinky).normalized(), slerpPointBetweenBack) * middleJoint.length;

	// Calculate closest point between upper & lower candidate
	Vector3f closestPoint = closesPointOnLine(point, upperCandidate, lowerCandidate);

	// Return normalized direction from upper to closest point
	return (closestPoint - upperPosition).normalized();
}