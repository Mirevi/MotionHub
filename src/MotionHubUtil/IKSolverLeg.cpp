#include "IKSolverLeg.h"

IKSolverLeg::IKSolverLeg() {

}

IKSolverLeg::IKSolverLeg(HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower) {

	// Create IKJoint objects from given joints
	upperJoint = IKJoint(upper);
	middleJoint = IKJoint(middle);
	lowerJoint = IKJoint(lower);

	// Add pointers for all IKJoints in vector
	joints.push_back(&upperJoint);
	joints.push_back(&middleJoint);
	joints.push_back(&lowerJoint);
}

void IKSolverLeg::init() {

	// Get joint name from upper joint
	Joint::JointNames upperJointName = upperJoint.joint->getJointName();

	// Cache if this is the LeftLeg/LeftArm Solver
	isLeft = false;
	if (upperJointName == Joint::UPLEG_L || upperJointName == Joint::ARM_L) {
		isLeft = true;
	}

	// Store upper, middle & lower position
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f middlePosition = middleJoint.getPosition();
	Vector3f lowerPosition = lowerJoint.getPosition();

	// Store chain direction vectors
	Vector3f upperToMiddle = middlePosition - upperPosition;
	Vector3f middleToLower = lowerPosition - middlePosition;

	// Set target to lower position & rotation
	targetPosition = lowerPosition;
	targetRotation = lowerJoint.getRotation();

	// Calculate default normal & store default normal in joint space
	Vector3f defaultNormal = upperToMiddle.cross(middleToLower);
	defaultLocalNormal = upperJoint.getRotation().inverse() * defaultNormal;

	// Initiate joints
	Quaternionf upperRotation = upperJoint.getRotation();
	Quaternionf middleRotation = middleJoint.getRotation();

	//upperSquaredNorm = upperToMiddle.squaredNorm();
	//middleSquaredNorm = middleToLower.squaredNorm();

	//upperLength = sqrtf(upperSquaredNorm);
	//middleLength = sqrtf(middleSquaredNorm);

	//rotationToSpace(upperRotation, lookRotation(upperToMiddle, defaultNormal));
	//rotationToSpace(middleRotation, lookRotation(middleToLower, defaultNormal));

	upperJoint.init(upperToMiddle, defaultNormal);
	middleJoint.init(middleToLower, defaultNormal);

	refresh(true);
}

void IKSolverLeg::refresh(bool overrideDefault) {

	// Save default state for joints if configured
	if (overrideDefault) {
		saveDefaultState();
	}

	// Store upper, middle & lower position
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f middlePosition = middleJoint.getPosition();
	Vector3f lowerPosition = lowerJoint.getPosition();

	Vector3f upperToMiddle = middlePosition - upperPosition;
	Vector3f middleToLower = lowerPosition - middlePosition;

	// Store middle to upper direction
	middleToUpper = upperPosition - middlePosition;

	// Update & Store legnth of joints
	upperJoint.setLength(upperToMiddle);
	middleJoint.setLength(middleToLower);
	length = (upperJoint.length + middleJoint.length);

	// Reset lastNormal
	lastNormal = Vector3f::Zero();

	// Reset hint
	hintPosition = Vector3f::Zero();
	hintRotation = Quaternionf::Identity();
	hasHint = false;
}

void IKSolverLeg::saveDefaultState() {

	// Save default state for all joints
	upperJoint.saveDefaultState();
	middleJoint.saveDefaultState();
	lowerJoint.saveDefaultState();

	// TODO: ZU IK Joint
	// Save lower default rotation
	invUpperDefaultRotation = upperJoint.joint->getGlobalRotation().inverse();
	invMiddleDefaultRotation = middleJoint.joint->getGlobalRotation().inverse();
	invLowerDefaultRotation = lowerJoint.joint->getGlobalRotation().inverse();
}

void IKSolverLeg::loadDefaultState() {

	// Load default state for all joints
	upperJoint.loadDefaultState();
	middleJoint.loadDefaultState();
	lowerJoint.loadDefaultState();
}

void IKSolverLeg::updateNormal() {


	// Is hint available & is not calibrationg
	if (!isCalibrating && hasHint) {
		// Rotate normal with Hint
		Vector3f hintNormal = hintRotation * invMiddleDefaultRotation * defaultLocalNormal;
		normal = hintNormal.normalized();

		return;
	}

	// Get normal from current rotation
	Quaternionf upperRotation = upperJoint.getRotation();
	Vector3f currentNormal = upperRotation * defaultLocalNormal;

	// Get target normal relative to lower default rotation 
	Quaternionf rotation = targetRotation * invLowerDefaultRotation;
	Vector3f targetNormal = rotation * defaultLocalNormal;

	Quaternionf fromTo = fromToRotation(upperRotation, rotation);
	normal = upperRotation.slerp(slerpTargetRotationDelta, fromTo * upperRotation) * defaultLocalNormal;

	// normalize normal
	normal = normal.normalized();
	lastNormal = normal;
}

void IKSolverLeg::solve(Vector3f position, Quaternionf rotation) {

	// TODO: Debug raus
	clearDebugLines();

	// TODO: position auf length beschränken
	/*
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f upperToTarget = position - upperPosition;
	if (upperToTarget.norm() >= (length * 0.98f)) {
		upperToTarget = upperToTarget.normalized() * length * 0.98f;
	}

	// Call solve from base class
	IKSolver::solve(upperToTarget + upperPosition, rotation);
	*/
	IKSolver::solve(position, rotation);

	// Restore default state
	loadDefaultState();

	// Store current position in each joint
	for (IKJoint* joint : joints) {
		joint->saveSolvedPosition();
	}

	// limit solve position to avoid stretching & twitching when target is out of reach
	constraintSolvePosition(upperJoint.getPosition());

	// update normal
	updateNormal();

	// Solve IK
	solve();

	// Constraint solution
	constraint();

	// Apply solution to joints
	apply();

	// Untwist lower joints based on target rotation
	untwist();
}

void IKSolverLeg::hint(Vector3f position, Quaternionf rotation) {

	hintPosition = position;
	hintRotation = rotation;

	hasHint = true;
}

void IKSolverLeg::solve() {

	// store start position
	startPosition = upperJoint.getSolvedPosition();

	// update solve position (shoulder modifies start position -> reach could be greater)
	constraintSolvePosition(startPosition);

	// generate axis for knee hint calculation
	Vector3f helpAxis = (solvePosition - startPosition).cross(normal);

	// Unit upper length multiplicator if not calibration
	//float upperMultiplicator = 1.001f;
	float upperMultiplicator = 1.001f;
	if (isCalibrating) {
		upperMultiplicator = 1.0f;
	}


	// get or calculate knee hint position
	Vector3f middleHint = hintPosition;
	if (!hasHint) {
		middleHint = calcInverseKinematic(startPosition, solvePosition, upperJoint.length * upperMultiplicator, middleJoint.length, helpAxis);
	}

	if (hasHint) {
		middleHint += (solvePosition - hintPosition).cross(normal).normalized() * 0.05f;
	}

	// Move middle position to knee hint
	Vector3f middlePosition = startPosition + (middleHint - startPosition).normalized() * upperJoint.length;
	middleJoint.setSolvedPosition(middlePosition);

	// Move lower position between knee hint & target
	Vector3f lowerPosition = middlePosition + (solvePosition - middlePosition).normalized() * middleJoint.length;
	lowerJoint.setSolvedPosition(lowerPosition);

	// init distance to target with "infinity"
	float lastDistance = FLT_MAX;

	// loop until max allowed iterations or a break condition is reached
	for (int iteration = 1; iteration <= iterations; iteration++) {

		// FABRIK algorithm
		forwardReach();
		backwardReach();

		// Calculate distance between target position & last joint position
		float distanceToTarget = distance(solvePosition, lowerJoint.getSolvedPosition());

		// Break if tolerance distance is reached
		if (distanceToTarget < distanceTolerance) {
			break;
		}

		// Break if distance stayed the same
		if (distanceToTarget == lastDistance) {
			break;
		}

		// Break if distance is lower than progress tolerance
		if (abs(distanceToTarget - lastDistance) < progressTolerance) {
			break;
		}

		// Store current distance to target
		lastDistance = distanceToTarget;
	}
}

void IKSolverLeg::constraint() {

	// Store solved positions of all 3 joints
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f middlePosition = middleJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Get the rotation limb axis from upper to lower joint
	Vector3f limbAxis = (lowerPosition - upperPosition).normalized();

	// Store current direction
	Vector3f currentDirection = (middlePosition - upperPosition).normalized();

	// Get direction the limb should point to
	Vector3f bendDirection = (solvePosition - upperPosition).cross(normal);

	// Get direction to hint if not calibrating
	if (!isCalibrating && hasHint) {

		Vector3f hintOffset = (targetPosition - hintPosition).cross(normal).normalized() * 0.05f;

		DebugPos2 = hintPosition + hintOffset;

		bendDirection = ((hintPosition + hintOffset) - upperPosition).normalized();
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

void IKSolverLeg::constraintSolvePosition(Vector3f startPosition) {

	// constraint solve position if distance is greater than length
	if (distance(startPosition, targetPosition) >= length) {
		solvePosition = startPosition + (targetPosition - startPosition).normalized() * length;
	}
}

void IKSolverLeg::apply() {

	// TODO: Use Actual Position (IKSolverArm)

	// Store middle position 
	Vector3f middlePosition = middleJoint.getSolvedPosition();

	// Rotate upper Joint
	Vector3f middleDirection = middlePosition - upperJoint.getSolvedPosition();
	upperJoint.setRotationTowards(middleDirection, normal);

	// Rotate middle Joint
	Vector3f lowerDirection = lowerJoint.getSolvedPosition() - middlePosition;
	middleJoint.setRotationTowards(lowerDirection);

	// TODO: preTwist(); ?

	// Rotate lower Joint
	lowerJoint.setRotation(targetRotation);
}

void IKSolverLeg::untwist() {

	return;

	// TODO: Wie Shoulder default rotation beruecksichtigen

	// TODO: confident angle? 180 flips

	// TODO: Debug raus
	if (lowerJoint.joint->getJointName() == Joint::FOOT_R) {
		return;
	}

	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		return;
	}

	// Store positions for all joints
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f middlePosition = middleJoint.getPosition();
	Vector3f lowerPosition = lowerJoint.getPosition();

	// Store rotations for all joints
	Quaternionf upperRotation = upperJoint.getRotation();
	Quaternionf middleRotation = middleJoint.getRotation();
	Quaternionf lowerRotation = lowerJoint.getRotation();

	// Untwist middle joint if configured
	if (middleUntwistWeight > 0) {

		//debugDrawLine(middlePosition, middlePosition + middleJoint.joint->getForward(), Vector3f(1.0f, 1.0f, 1.0f));

		// Store inverse middle rotation
		Quaternionf invMiddleRotation = middleRotation.inverse();

		// Store middle joint right, up & forward axis
		Vector3f middleRight = middleRotation * Vector3f(1, 0, 0);
		Vector3f middleUp = middleRotation * Vector3f(0, 1, 0);
		Vector3f middleForward = middleRotation * Vector3f(0, 0, 1);

		// Store rotational difference between lower & middle joint
		Quaternionf diff = lowerRotation * invMiddleRotation;

		// Decompose twist in relation to middle forward axis
		Quaternionf twist = decomposeTwist(diff, middleForward);

		// Lerp twist by configured weight
		Quaternionf middleTwist = middleRotation.slerp(middleUntwistWeight, twist);

		/*
		// old (without to child vector)
		Vector3f twistMiddleRight = middleTwist * middleRight;
		orthoNormalize(middleForward, twistMiddleRight);
		float angle = -signedAngle(middleRight, twistMiddleRight, middleForward);
		middleRotation = angleAxis(angle, middleUp) * middleRotation; // * middleRotation
		*/

		// Apply twist to middle right axis & ortho normalize it
		Vector3f twistMiddleRight = middleTwist * middleRight;
		orthoNormalize(middleForward, twistMiddleRight);

		//float angle = -signedAngle(middleRight, twistMiddleRight, middleForward);

		// Calculate angle between current right and twisted right on forward
		float angle = signedAngle(middleRight, twistMiddleRight, middleForward);

		// Calculate rotation axis
		Vector3f middleToLowerNorm = (lowerPosition - middlePosition).normalized();

		// Apply Rotation
		middleRotation = angleAxis(angle, middleToLowerNorm) * middleRotation;
		middleJoint.setRotation(middleRotation);

		// Restore child
		//lowerJoint.setRotation(lowerRotation);
	}

	// Untwist upper joint if configured
	if (upperUntwistWeight > 0) {

		// Store upper joint up & forward axis
		Vector3f upperUp = upperRotation * Vector3f(0, 1, 0);
		Vector3f upperForward = upperRotation * Vector3f(0, 0, 1);

		// Store rotational difference between middle & upper joint
		Quaternionf diff = middleRotation * upperRotation.inverse();

		// Decompose twist in relation to upper up axis
		Quaternionf twist = decomposeTwist(diff, upperUp);

		// Lerp twist by configured weight
		Quaternionf upperTwist = upperRotation.slerp(upperUntwistWeight, twist);

		// Apply twist to upper forward axis & ortho normalize it
		Vector3f twistUpperForward = upperTwist * upperForward;
		orthoNormalize(upperUp, twistUpperForward);

		// Calculate angle between current forard and twisted forward on up
		float angle = -signedAngle(upperForward, twistUpperForward, upperUp);

		if (lowerJoint.joint->getJointName() == Joint::FOOT_R) {
			//angle = 0;
		}

		Vector3f upperToMiddleNorm = (middlePosition - upperPosition).normalized();

		upperRotation = angleAxis(angle, upperToMiddleNorm) * upperRotation; // * upperRotation
		upperJoint.setRotation(upperRotation);

		// Restore child rotations
		middleJoint.setRotation(middleRotation);
	}

	// Restore child rotation
	lowerJoint.setRotation(lowerRotation);
}