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


	handToBack = Vector3f(1, 0, 0);
	if (isLeft) {
		handToBack = Vector3f(-1, 0, 0);
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

	lastShoulderRotation = shoulderJoint.getRotation();

	lastShoulderToHint = middleJoint.getPosition() - shoulderJoint.getPosition();

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

	if (hasHint) {
		IKSolverLeg::updateNormal();
	}
	else {
		// Get normal from current rotation
		Quaternionf upperRotation = upperJoint.getRotation();
		Vector3f currentNormal = upperRotation * defaultLocalNormal;

		// Get target normal relative to lower default rotation 
		Quaternionf rotation = targetRotation * invLowerDefaultRotation;
		Vector3f targetNormal = rotation * defaultLocalNormal;

		float angle = angleBetween(upperRotation, rotation);
		Quaternionf fromTo = fromToRotation(upperRotation, rotation);

		Vector3f slerpNormal = upperRotation.slerp(slerpTargetRotationDelta, fromTo * upperRotation) * defaultLocalNormal;

		float angleDelta = (360.0f - angle) / angle;
		Vector3f invSlerpNormal = upperRotation.slerp(angleDelta * -slerpTargetRotationDelta, fromTo * upperRotation) * defaultLocalNormal;

		//if(!isLeft)
		//Console::log(toString(angleBetween(lastNormal, slerpNormal)) + " <= " + toString(angleBetween(lastNormal, invSlerpNormal)));

		//if (angleBetween(currentNormal, targetNormal) <= 30.0f || lastNormal.isApprox(Vector3f::Zero())) {
		if (lastNormal.isApprox(Vector3f::Zero())) {
			normal = slerpNormal;
		}
		else if (angleBetween(lastNormal, slerpNormal) <= angleBetween(lastNormal, invSlerpNormal)) {
			normal = slerpNormal;
		}
		else {
			normal = invSlerpNormal;
		}
		
		// normalize normal
		normal = normal.normalized();
		lastNormal = normal;
	}
}

void IKSolverArm::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolverLeg::solve(position, rotation);
}

void IKSolverArm::solve() {

	shoulderJoint.loadDefaultState();

	// TODO: Ueberarbeiten
	if (!isCalibrating && hasHint) {

		// Read shoulder positon & relative shoulder rotation
		Vector3f shoulderPosition = shoulderJoint.getPosition();
		Quaternionf localShoulderRotation = shoulderJoint.getRotation() * invShoulderDefaultRotation;

		// Calculate vectors from shoulder to current elbow & new elbow (from last frame)
		Vector3f upperPosition = upperJoint.getPosition();
		Vector3f toUpper = upperPosition - shoulderPosition;

		Vector3f upperHint = hintPosition + (hintRotation * middleToUpper);

		// lerp hint / shoulder pos
		Vector3f toUpperHint = lerp(upperPosition, upperHint, 0.9f) - shoulderPosition;

		// Calculate rotation from current elbow to new elbow position 
		Quaternionf solvedRotation = fromToRotation(toUpper, toUpperHint);

		// Extract local shoulder y- & z-axis
		Vector3f yAxis = localShoulderRotation * Vector3f(0, 1, 0);
		Vector3f zAxis = localShoulderRotation * Vector3f(0, 0, 1);

		if (isLeft) {

			Vector3f reachableHint = upperPosition + (hintPosition - upperPosition).normalized() * upperJoint.length;

			Vector3f shoulderToReachableHint = reachableHint - shoulderPosition;
			Vector3f shoulderToHint = hintPosition - shoulderPosition;

			solvedRotation = fromToRotation(shoulderToReachableHint, shoulderToHint);

			//DebugPos1 = reachableHint;
			//DebugPos2 = hintPosition;

			//Console::log(toString(yAngle) + "    " + toString(zAngle));
			//rotation = solvedRotation;
		}

		// Decompose y rotation & calculate y angle
		Quaternionf yRotation = decomposeTwist(solvedRotation, yAxis);
		//float yAngle = signedAngle(zAxis, yRotation * zAxis, yAxis);

		// Decompose z rotation & calculate z angle
		Quaternionf zRotation = decomposeTwist(solvedRotation, zAxis);
		//float zAngle = signedAngle(yAxis, zRotation * yAxis, zAxis);

		Quaternionf rotation = (yRotation * zRotation);

		shoulderJoint.setRotation(rotation * shoulderJoint.getRotation());
		for (IKJoint* joint : joints) {
			joint->saveSolvedPosition();
		}
	}

	else if (!isCalibrating) {


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
				yDeltaAngle = mapClamp(yAngle, 0, 80, 0, shoulderForwardYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -80, 0, -shoulderForwardYAngle - handYAngle, 0);
			}
		}
		else {
			if (yAngle >= 0) {
				yDeltaAngle = mapClamp(yAngle, 0, 80, 0, shoulderBackYAngle + handYAngle);
			}
			else {
				yDeltaAngle = mapClamp(yAngle, -80, 0, -shoulderBackYAngle - handYAngle, 0);
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
				zDeltaAngle = mapClamp(zAngle, 0, 80, 0, shoulderUpZAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -80, 0, -shoulderUpZAngle - handZAngle, 0);
			}
		}
		else {
			if (zAngle >= 0) {
				zDeltaAngle = mapClamp(zAngle, 0, 80, 0, shoulderDownZAngle + handZAngle);
			}
			else {
				zDeltaAngle = mapClamp(zAngle, -80, 0, -shoulderDownZAngle - handZAngle, 0);
			}
		}

		// Clamp z rotation by shoulder-elbow angle & shoulder-hand distance
		zRotation = rotateTowards(Quaternionf::Identity(), zRotation, abs(zDeltaAngle));

		// Get rotation from y- & z-rotation
		Quaternionf rotation = (yRotation * zRotation);


		Quaternionf newShoulderRotation = rotation * shoulderJoint.getRotation();
		// Set upper joint solved position & shoulder rotation
		//upperJoint.setSolvedPosition(direction + shoulderPosition);
		shoulderJoint.setRotation(lastShoulderRotation.slerp(0.5f, newShoulderRotation));

		if (hasHint) {
			shoulderJoint.setRotation(newShoulderRotation);

			Vector3f upperPosition = upperJoint.getPosition();
			Vector3f newUpperToHint = hintPosition - upperJoint.getPosition();
			float distanceToHint = distance(hintPosition, upperPosition);

			Vector3f newUp = hintPosition + (upperPosition - hintPosition).normalized() * upperJoint.length;

			Vector3f shoulderToUp = upperPosition - shoulderPosition;
			Vector3f shoulderToNewUp = newUp - shoulderPosition;

			rotation = fromToRotation(shoulderToUp, shoulderToNewUp);

			//rotation = Quaternion::Identity().slerp(0.9f, rotation);

			newShoulderRotation = rotation * shoulderJoint.getRotation();

			shoulderJoint.setRotation(newShoulderRotation);
		}

		lastShoulderRotation = newShoulderRotation;

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

	Vector3f bendDirection = currentDirection;

	if (!isLeft) {
		bendDirection += (middlePosition - upperPosition).normalized() * 1.2f;

		bendDirection += targetRotation * (handToBack * 0.25f);
		bendDirection += Vector3f(0, -1.0f, 0);

		//DebugPos1 = upperPosition + currentDirection + Vector3f(0, -0.75f, 0);
		//DebugPos3 = upperPosition + currentDirection + targetRotation * (handToBack * 0.5f);
	}
	else {
		bendDirection += (middlePosition - upperPosition).normalized() * 1.0f;

		bendDirection += targetRotation * (handToBack * 0.25f);
		bendDirection += Vector3f(0, -1.0f, 0);

		//DebugPos1 = upperPosition + currentDirection + Vector3f(0, -0.75f, 0);
		//DebugPos3 = upperPosition + currentDirection + targetRotation * (handToBack * 0.1f);
	}

	// Ortho normalize both directions on limb axis
	orthoNormalize(limbAxis, currentDirection);
	orthoNormalize(limbAxis, bendDirection);

	// Calculate angle difference on limb axis & create rotation on axis
	float angle = signedAngle(currentDirection, bendDirection, limbAxis);
	Quaternionf rotation = angleAxis(angle, limbAxis);

	// Rotate the middle bone using the angle
	Vector3f newMiddlePosition = angleAxis(angle, limbAxis) * (middlePosition - upperPosition) + upperPosition;
	middleJoint.setSolvedPosition(newMiddlePosition);
}

void IKSolverArm::apply() {

	IKSolverLeg::apply();
}

void IKSolverArm::untwist() {

	// TODO: Untwist neu schreiben
	//IKSolverLeg::untwist();
}
