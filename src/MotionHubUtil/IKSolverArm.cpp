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

	// Slerp normal between upper & calculated leg normal
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

		lastShoulderRotation = newShoulderRotation;

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

	// Store solved positions of all 3 joints
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f middlePosition = middleJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Get the rotation limb axis from upper to lower joint
	Vector3f limbAxis = (lowerPosition - upperPosition).normalized();

	// Store current direction
	Vector3f currentDirection = (middlePosition - upperPosition).normalized();
	currentDirection += (solvePosition - upperPosition).cross(normal).normalized() * 0.01f;


	DebugPos4 = upperPosition + (Quaternionf::Identity().slerp(0.8f, fromToRotation(currentDirection, -chest->getUp())) * currentDirection).normalized() * upperJoint.length;

	Vector3f bendDirection = lerp(currentDirection, (upperPosition - chest->getGlobalPosition()).normalized(), 0.5f);

	// TODO: bend direction nicht lerpen und so uebernehmen?

	DebugPos1 = upperPosition + currentDirection.normalized() * upperJoint.length;
	DebugPos2 = upperPosition + (upperPosition - chest->getGlobalPosition()).normalized() * upperJoint.length;
	DebugPos3 = upperPosition + bendDirection.normalized() * upperJoint.length;

	Vector3f line_start = upperPosition + bendDirection.normalized() * upperJoint.length;

	Quaternionf toDown = fromToRotation(bendDirection, -chest->getUp());
	bendDirection = (Quaternionf::Identity().slerp(0.8f, toDown) * bendDirection);

	Vector3f line_end = upperPosition + bendDirection.normalized() * upperJoint.length;

	Vector3f toHandJoint = targetRotation * (handToJoint * 0.5f);
	Vector3f toHandPinky = targetRotation * (handToPinky * 0.2f);

	Vector3f chestPosition = chest->getGlobalPosition();
	Vector3f chestForward = chest->getForward();
	Vector3f chestUp = chest->getUp();

	Vector3f chestToTarget = solvePosition - chestPosition;
	Vector3f chestToTargetOnUp = projectOnPlane(chestToTarget, chestUp);

	float angleToBack = signedAngle(-chestForward, chestToTargetOnUp, chestUp);
	if (isLeft) {
		angleToBack = fabs(angleToBack);
	}
	else {
		// TODO: Testen
		angleToBack = -fabs(angleToBack);
	}

	Vector3f towardsBack = angleAxis(angleToBack * 0.2f, chestUp) * chestToTarget.normalized() * (upperJoint.length * 2.0f) + chestPosition;
	Vector3f towardsBackOnUp = projectOnPlane(towardsBack - solvePosition, chestUp).normalized() * middleJoint.length;

	//DebugPos1 = line_start;
	//DebugPos3 = line_end;

	line_start += towardsBackOnUp + Vector3f(0, middleJoint.length * 0.5f, 0);
	line_end += (towardsBackOnUp * 0.5f) + Vector3f(0, -middleJoint.length, 0);

	//DebugPos2 = line_start;
	//DebugPos4 = line_end;

	Vector3f between = lerp(line_start, line_end, 0.6f);
	Vector3f point = solvePosition + slerp((between - solvePosition).normalized(), (toHandJoint + toHandPinky).normalized(), 0.5f) * middleJoint.length;

	Vector3f closestPoint = closesPointOnLine(point, line_start, line_end);

	//closestPoint = lerp(closestPoint, line_end, 0.5f);
	//closestPoint = lerp(closestPoint, line_end, 0.3f);

	Vector3f hintOffset = Vector3f::Zero();

	Vector3f shoulderToUpper = (upperPosition - shoulderJoint.getPosition()).normalized();

	float angle3 = angleBetween(projectOnPlane(limbAxis, chestUp), -chestForward);

	if (angle3 <= 120.0f) {
		float mappedAngle = mapClamp(angle3, 120, 0, 0, 1);
		hintOffset = -chestForward.normalized() * mappedAngle;

		hintOffset += shoulderToUpper * (mappedAngle * 0.5f);

		float angle4 = signedAngle(projectOnPlane(limbAxis, chestForward), shoulderToUpper, chestForward);

		if (isLeft && angle4 <= 0.0f) {
			//Console::log("L");

			float map2 = mapClamp(fabs(angle4), 0, 90, 0, mappedAngle * 0.5f);
			hintOffset += -chestUp.normalized() * map2;

			hintOffset += shoulderToUpper * (map2 * 0.5f);

		}
		else if (!isLeft && angle4 >= 0.0f) {
			//Console::log("R");
			float map2 = mapClamp(angle4, 0, 90, 0, mappedAngle * 0.5f);
			hintOffset += -chestUp.normalized() * map2;

			hintOffset += shoulderToUpper * (map2 * 0.5f);
		}
	}

	if (DebugBool2) {
		//bendDirection = (closestPoint + hintOffset) - upperPosition;
		bendDirection = (closestPoint)-upperPosition;
		bendDirection = bendDirection.normalized();
	}
	else {
		bendDirection = (closestPoint + hintOffset) - upperPosition;
		bendDirection = bendDirection.normalized();
	}

	if (DebugBool3) {
		bendDirection = lerp(currentDirection, (upperPosition - chest->getGlobalPosition()).normalized(), 0.75f);

		Vector3f chestDown = -chest->getUp();
		Quaternionf toDown = fromToRotation(bendDirection, -chest->getUp());
		bendDirection = (Quaternionf::Identity().slerp(0.5f, toDown) * bendDirection);

		//bendDirection += Vector3f(0, -0.5f, 0);
		bendDirection += targetRotation * (handToJoint * 0.2f);
	}

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