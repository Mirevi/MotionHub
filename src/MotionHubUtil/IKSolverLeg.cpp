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
	lastMiddleNormal = Vector3f::Zero();

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

/*
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
	Vector3f upperNormal = upperRotation * defaultLocalNormal;

	// Get target normal relative to lower default rotation
	Quaternionf rotation = targetRotation * invLowerDefaultRotation;
	Vector3f targetNormal = rotation * defaultLocalNormal;

	Quaternionf fromTo = fromToRotation(upperRotation, rotation);
	normal = upperRotation.slerp(slerpTargetRotationDelta, fromTo * upperRotation) * defaultLocalNormal;

	// normalize normal
	normal = normal.normalized();
	lastNormal = normal;
}
*/

void IKSolverLeg::updateNormal() {

	// Is hint available & is not calibrationg
	if (!isCalibrating && hasHint) {
		// Rotate normal with Hint
		Vector3f hintNormal = hintRotation * invMiddleDefaultRotation * defaultLocalNormal;
		normal = hintNormal.normalized();

		return;
	}

	// Get upper normal from current rotation
	Quaternionf upperRotation = upperJoint.getRotation();
	Vector3f upperNormal = upperRotation * defaultLocalNormal;

	// Get target normal relative to lower default rotation 
	Quaternionf rotation = targetRotation * invLowerDefaultRotation;
	Vector3f targetNormal = rotation * defaultLocalNormal;

	// Create rotation axis from upper to target
	Vector3f upAxis = (solvePosition - upperJoint.getPosition()).normalized();

	// Create look rotation to upper normal & target normal
	Quaternionf lookToUpper = lookRotation(upperNormal, upAxis);
	Quaternionf lookToTarget = lookRotation(targetNormal, upAxis);

	// Create relative rotation from upper to target
	Quaternionf fromTo = fromToRotation(lookToUpper, lookToTarget);

	// Cache identity rotation for 2 slerps
	Quaternionf identity = Quaternionf::Identity();

	// Rotate normal towards target with configured rotation delta
	Vector3f slerpNormal = identity.slerp(slerpTargetRotationDelta, fromTo) * upperNormal;

	// Rotate normal investe towards target with configured rotation delta
	float angle = angleBetween(lookToUpper, lookToTarget);

	float angleDelta = (360.0f - angle) / angle;
	Vector3f invSlerpNormal = identity.slerp(angleDelta * -slerpTargetRotationDelta, fromTo) * upperNormal;

	//DebugPos1 = targetPosition + (lookToUpper * Vector3f(0, 0, 0.1f));
	//DebugPos2 = targetPosition + (lookToTarget * Vector3f(0, 0, 0.1f));

	angle = angleBetween(lookToUpper * Vector3f(0, 0, 1), lookToTarget * Vector3f(0, 0, 1));

	// Is last normal not initialized or an confident angle?
	if (lastNormal.isApprox(Vector3f::Zero()) || angle <= 45.0f) {
		normal = slerpNormal;
		//DebugPos3 = targetPosition + invSlerpNormal.normalized() * 0.1f;
	}
	// Is Normal closer to last normal?
	else if (angleBetween(lastNormal, slerpNormal) <= angleBetween(lastNormal, invSlerpNormal)) {
		normal = slerpNormal;
		//DebugPos3 = targetPosition + invSlerpNormal.normalized() * 0.1f;
	}
	// Inverse is closer to last normal
	else {
		normal = invSlerpNormal;
		//DebugPos3 = targetPosition + slerpNormal.normalized() * 0.1f;
	}

	//DebugPos4 = targetPosition + normal.normalized() * 0.1f;
	//DebugPos3 = DebugPos4;

	// normalize normal
	normal = normal.normalized();
	lastNormal = normal;
}

void IKSolverLeg::solve(Vector3f position, Quaternionf rotation) {

	// TODO: Debug raus
	clearDebugLines();

	// Call solve from base class
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

		bendDirection = (hintPosition + hintOffset) - upperPosition;
	}

	bendDirection = bendDirection.normalized();

	// Ortho normalize both directions on limb axis
	orthoNormalize(limbAxis, currentDirection);
	orthoNormalize(limbAxis, bendDirection);

	// projectOnPlane anstatt orthoNormalize?

	// Calculate angle difference on limb axis & create rotation on axis
	float angle = signedAngle(currentDirection, bendDirection, limbAxis);
	Quaternionf rotation = angleAxis(angle, limbAxis);

	// Rotate the middle bone using the angle
	Vector3f newMiddlePosition = rotation * (middlePosition - upperPosition) + upperPosition;
	middleJoint.setSolvedPosition(newMiddlePosition);
}

void IKSolverLeg::constraintSolvePosition(Vector3f startPosition) {

	// constraint solve position if distance is greater than length
	if (distance(startPosition, targetPosition) >= length) {
		solvePosition = startPosition + (targetPosition - startPosition).normalized() * length;
	}
}

void IKSolverLeg::apply() {

	// Rotate upper Joint to middle
	Vector3f middleDirection = middleJoint.getSolvedPosition() - upperJoint.getSolvedPosition();

	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
		upperJoint.setRotationTowards(middleDirection, normal);
	}
	else {
		Vector3f upperNormal = projectOnPlane(normal, middleDirection.normalized());
		upperJoint.setRotationTowards(middleDirection, upperNormal);
	}

	// Calculate normal for middle joint (between upper & lower)
	Quaternionf middleRotation = middleJoint.getRotation();
	Vector3f middleNormal = middleRotation * defaultLocalNormal;

	// Get target normal relative to lower default rotation 
	Quaternionf rotation = targetRotation * invLowerDefaultRotation;
	Vector3f targetNormal = rotation * defaultLocalNormal;

	// Create rotation axis from upper to target
	Vector3f upAxis = (solvePosition - middleJoint.getPosition()).normalized();

	// Create look rotation to current normal & target normal
	Quaternionf lookToMiddle = lookRotation(middleNormal, upAxis);
	Quaternionf lookToTarget = lookRotation(projectOnPlane(targetNormal, upAxis), upAxis);

	// TODO: Testen ob project on plane ergebis verbessert
	if (DebugBool3) {
		lookToTarget = lookRotation(targetNormal, upAxis);
	}

	// Create relative rotation from middle to target
	Quaternionf fromTo = fromToRotation(lookToMiddle, lookToTarget);

	// Rotate normal towards target with configured rotation delta
	Vector3f slerpNormal = Quaternionf::Identity().slerp(0.3f, fromTo) * middleNormal;

	// Rotate normal investe towards target with configured rotation delta
	float angle = angleBetween(lookToMiddle, lookToTarget);
	float angleDelta = (360.0f - angle) / angle;
	Vector3f invSlerpNormal = Quaternionf::Identity().slerp(angleDelta * -0.3f, fromTo) * middleNormal;

	// Update angle with both vectors on plane
	lookToMiddle = lookRotation(projectOnPlane(middleNormal, upAxis), upAxis);
	lookToTarget = lookRotation(projectOnPlane(targetNormal, upAxis), upAxis);
	angle = angleBetween(lookToMiddle * Vector3f(0, 0, 1), lookToTarget * Vector3f(0, 0, 1));

	// Is last normal not initialized or an confident angle?
	if (lastMiddleNormal.isApprox(Vector3f::Zero()) || angle <= 45.0f) {
		middleNormal = slerpNormal;
	}
	// Is Normal closer to last normal?
	else if (angleBetween(lastMiddleNormal, slerpNormal) <= angleBetween(lastMiddleNormal, invSlerpNormal)) {
		middleNormal = slerpNormal;
	}
	// Inverse is closer to last normal
	else {
		middleNormal = invSlerpNormal;
	}

	// normalize middle normal
	middleNormal = middleNormal.normalized();
	lastMiddleNormal = middleNormal;

	if (DebugBool2) {
		middleNormal = middleRotation * defaultLocalNormal;
	}

	// Rotate middle Joint to solve position
	Vector3f lowerDirection = solvePosition - middleJoint.getPosition();
	middleNormal = projectOnPlane(middleNormal, lowerDirection.normalized());
	middleJoint.setRotationTowards(lowerDirection, middleNormal);

	// Rotate lower Joint to target rotation
	lowerJoint.setRotation(targetRotation);
}