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

	// TODO: Achsen bestimmen
	// - Handflaeche zu Mittelfinger
	// - Handflaeche zu Daumen

	handToPinky = Vector3f(0, 0, 1);
	if (isLeft) {
		handToPinky = Vector3f(0, 0, -1);
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
	defaultShoulderHandDistance = (lowerJoint.getLocalPosition() - shoulderJoint.getPosition()).norm();
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

	slerpTargetRotationDelta = 0.25f;

	IKSolverLeg::updateNormal();
}

void IKSolverArm::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolverLeg::solve(position, rotation);
}

void IKSolverArm::solve() {

	//IKSolverLeg::solve();
	//return;

	if (!isCalibrating && hasHint) {

		shoulderJoint.loadDefaultState();

		// Read shoulder positon & relative shoulder rotation
		Vector3f shoulderPosition = shoulderJoint.getPosition();
		Quaternionf localShoulderRotation = shoulderJoint.getRotation() * invShoulderDefaultRotation;

		// Calculate vectors from shoulder to current elbow & new elbow (from last frame)
		Vector3f toUpper = upperJoint.getPosition() - shoulderPosition;

		Vector3f upperHint = hintPosition + (hintRotation * middleToUpper);

		// lerp hint / shoulder pos

		Vector3f toUpperHint = upperHint - shoulderPosition;

		// Calculate rotation from current elbow to new elbow position 
		Quaternionf solvedRotation = fromToRotation(toUpper, toUpperHint);

		// Extract local shoulder y- & z-axis
		Vector3f yAxis = localShoulderRotation * Vector3f(0, 1, 0);
		Vector3f zAxis = localShoulderRotation * Vector3f(0, 0, 1);

		// Decompose y rotation & calculate y angle
		Quaternionf yRotation = decomposeTwist(solvedRotation, yAxis);
		float yAngle = signedAngle(zAxis, yRotation * zAxis, yAxis);

		// Decompose z rotation & calculate z angle
		Quaternionf zRotation = decomposeTwist(solvedRotation, zAxis);
		float zAngle = signedAngle(yAxis, zRotation * yAxis, zAxis);

		Quaternionf rotation = (yRotation * zRotation);

		shoulderJoint.setRotation(rotation * shoulderJoint.getRotation());
		for (IKJoint* joint : joints) {
			joint->saveSolvedPosition();
		}
	}

	else if (!isCalibrating && solveShoulder) {

		shoulderJoint.loadDefaultState();

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
		Vector3f middleHint = calcInverseKinematic(shoulderPosition, shoulderSolvePosition, upperJoint.length + shoulderDelta, middleJoint.length + shoulderDelta, helpAxis);

		Vector3f toHint = middleHint - shoulderPosition;

		Vector3f toNewMiddle = slerp(lastShoulderToHint, toHint, 0.5f);

		// Calculate rotation from current elbow to new elbow position 
		Quaternionf solvedRotation = fromToRotation(toMiddle, toNewMiddle);

		lastShoulderToHint = toHint;

		// Extract local shoulder y- & z-axis
		//Vector3f xAxis = localShoulderRotation * Vector3f(1, 0, 0);
		Vector3f yAxis = localShoulderRotation * Vector3f(0, 1, 0);
		Vector3f zAxis = localShoulderRotation * Vector3f(0, 0, 1);

		// Decompose x rotation & calculate y angle
		//Quaternionf xRotation = decomposeTwist(solvedRotation, xAxis);

		// Decompose y rotation & calculate y angle
		Quaternionf yRotation = decomposeTwist(solvedRotation, yAxis);
		float yAngle = signedAngle(zAxis, yRotation * zAxis, yAxis);

		// Decompose z rotation & calculate z angle
		Quaternionf zRotation = decomposeTwist(solvedRotation, zAxis);
		float zAngle = signedAngle(yAxis, zRotation * yAxis, zAxis);

		// Calculate shoulder to target and project on chest up & forward
		Vector3f shoulderToTarget = targetPosition - shoulderPosition;
		Vector3f shoulderToTargetOnUp = projectOnPlane(shoulderToTarget, chest->getUp());
		Vector3f shoulderToTargetOnForward = projectOnPlane(shoulderToTarget, chest->getForward());

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

		// Clamp y Angle by config
		float yDeltaAngle;
		if (isForward) {
			if (yAngle >= 0) {
				yDeltaAngle = mapClamp(yAngle, 0, 70, 0, shoulderForwardYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -70, 0, -shoulderForwardYAngle - handYAngle, 0);
			}
		}
		else {
			if (yAngle >= 0) {
				yDeltaAngle = mapClamp(yAngle, 0, 70, 0, shoulderBackYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -70, 0, -shoulderBackYAngle - handYAngle, 0);
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
				zDeltaAngle = mapClamp(zAngle, 0, 70, 0, shoulderUpZAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -70, 0, -shoulderUpZAngle - handZAngle, 0);
			}
		}
		else {
			if (zAngle >= 0) {
				zDeltaAngle = mapClamp(zAngle, 0, 70, 0, shoulderDownMaxAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -70, 0, -shoulderDownMaxAngle - handZAngle, 0);
			}
		}

		// Clamp z rotation by shoulder-elbow angle & shoulder-hand distance
		zRotation = rotateTowards(Quaternionf::Identity(), zRotation, abs(zDeltaAngle));

		// Get rotation from y- & z-rotation
		Quaternionf rotation = (yRotation * zRotation);

		bool solve = true;
		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
			solve = false;
		}

		if (solve) {
			// Set upper joint solved position & shoulder rotation
			//upperJoint.setSolvedPosition(direction + shoulderPosition);
			shoulderJoint.setRotation(rotation * shoulderJoint.getRotation());
			for (IKJoint* joint : joints) {
				joint->saveSolvedPosition();
			}
		}
	}

	IKSolverLeg::solve();
}

void IKSolverArm::constraint() {

	IKSolverLeg::constraint();
}

void IKSolverArm::apply() {

	IKSolverLeg::apply();
}

void IKSolverArm::untwist() {

	// TODO: Untwist neu schreiben
	//IKSolverLeg::untwist();
}
