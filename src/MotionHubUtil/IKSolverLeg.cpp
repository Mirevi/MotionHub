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

/*
void IKSolverLeg::seToe(HierarchicJoint* joint) {

	toe = joint;
}
*/

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

	upperJoint.init(upperToMiddle, defaultNormal);
	middleJoint.init(middleToLower, defaultNormal);

	refresh(true);
}

void IKSolverLeg::refresh(bool overrideDefault) {

	// Save default state for joints if configured
	if (overrideDefault) {
		saveDefaultState();

		/*if (toe != nullptr) {
			toeCalibratedY = toe->getGlobalPosition().y();
		}*/
	}
	// Store upper, middle & lower position
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f middlePosition = middleJoint.getPosition();
	Vector3f lowerPosition = lowerJoint.getPosition();

	// Update & Store length of joints
	upperJoint.setLength(middlePosition - upperPosition);
	middleJoint.setLength(lowerPosition - middlePosition);
	length = (upperJoint.length + middleJoint.length);

	// Reset lastNormal
	lastNormal = Vector3f::Zero();
	lastMiddleNormal = Vector3f::Zero();
	lastUpperToTarget = upperJoint.getRotation().inverse() * (lowerJoint.getPosition() - upperJoint.getPosition());

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
	invMiddleDefaultRotation = middleJoint.getRotation().inverse();
	invLowerDefaultRotation = lowerJoint.getRotation().inverse();

	/*if (toe != nullptr) {
		toeDefaultPosition = toe->getLocalPosition();
		toeDefaultRotation = toe->getLocalRotation();
	}*/

}

void IKSolverLeg::loadDefaultState() {

	// Load default state for all joints
	upperJoint.loadDefaultState();
	middleJoint.loadDefaultState();
	lowerJoint.loadDefaultState();

	/*if (toe != nullptr) {
		toe->setLocalPosition(toeDefaultPosition);
		toe->setLocalRotation(toeDefaultRotation);
	}*/
}

void IKSolverLeg::updateNormal() {

	// TODO: slerp/invSlerp auf Plausibilitaet pruefen?

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

	// Create axis from upper joint to target
	Vector3f upperPosition = upperJoint.getPosition();
	Vector3f axis = (solvePosition - upperPosition).normalized();

	if (angleBetween(targetForward, axis) >= 90) {
	}

	// Rotate upperNormal towards axis
	Vector3f upperToChild = (middleJoint.getPosition() - upperPosition).normalized();
	upperNormal = fromToRotation(upperToChild, axis) * upperNormal;

	// Rotate tagetNormal towards axis
	targetNormal = fromToRotation(targetForward, axis) * targetNormal;

	// Create look rotations for upper & target normals
	Quaternionf lookToUpper = lookRotation(upperNormal, axis);
	Quaternionf lookToTarget = lookRotation(targetNormal, axis);

	// Create from upper to target look rotation
	Quaternionf fromTo = fromToRotation(lookToUpper, lookToTarget);

	// Slerp from identity towards target
	Quaternionf identity = Quaternionf::Identity();
	Vector3f slerpNormal = identity.slerp(upperNormalToTargetDelta, fromTo) * upperNormal;

	// Slerp from identity towards target in reverse direction
	float angle = angleBetween(identity, fromTo);
	float angleDelta = (360.0f - angle) / fmax(angle, powf(0.1f, FLT_DECIMAL_DIG));
	Vector3f invSlerpNormal = identity.slerp(angleDelta * -upperNormalToTargetDelta, fromTo) * upperNormal;

	// Create direction towards last
	Vector3f upperToLastTarget = upperRotation * lastUpperToTarget;

	// Create direction to last target for each last, slerp & inverse slerp
	Vector3f lastBendDirection = (upperToLastTarget).cross(lastNormal).normalized();
	Vector3f bendDirectionA = (upperToLastTarget).cross(slerpNormal).normalized();
	Vector3f bendDirectionB = (upperToLastTarget).cross(invSlerpNormal).normalized();

	Vector3f lastMiddle = upperPosition + lastBendDirection;
	Vector3f middleA = upperPosition + bendDirectionA;
	Vector3f middleB = upperPosition + bendDirectionB;

	if (lastNormal.isApprox(Vector3f::Zero())) {
		normal = slerpNormal.normalized();
	}
	// use closest normal to last direction
	else if (distance(middleA, lastMiddle) < distance(middleB, lastMiddle)) {
		normal = slerpNormal.normalized();
	}
	else {
		normal = invSlerpNormal.normalized();
	}


	// Save last normal & direction to target for next iteration
	lastNormal = normal;
	lastUpperToTarget = upperRotation.inverse() * (solvePosition - upperPosition);
}

void IKSolverLeg::solve(Vector3f position, Quaternionf rotation) {

	targetForward = rotation * Vector3f(0, 0, 1);

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
	Vector3f upperNormal = projectOnPlane(normal, middleDirection.normalized());
	upperJoint.setRotationTowards(middleDirection, upperNormal);

	// Calculate normal for middle joint (between upper & lower)
	Quaternionf middleRotation = middleJoint.getRotation();
	Vector3f middleNormal = middleRotation * defaultLocalNormal;

	// Get target normal relative to lower default rotation 
	Quaternionf rotation = targetRotation * invLowerDefaultRotation;
	Vector3f targetNormal = rotation * defaultLocalNormal;

	// Create rotation axis from upper to target
	Vector3f axis = (solvePosition - middleJoint.getPosition()).normalized();

	// Rotate target forward to axis
	targetNormal = fromToRotation(targetForward, axis) * targetNormal;

	// Create look rotation to current normal & target normal
	Quaternionf lookToMiddle = lookRotation(middleNormal, axis);
	Quaternionf lookToTarget = lookRotation(projectOnPlane(targetNormal, axis), axis);

	// Create relative rotation from middle to target
	Quaternionf fromTo = fromToRotation(lookToMiddle, lookToTarget);

	// Rotate normal towards target with configured rotation delta
	middleNormal = Quaternionf::Identity().slerp(middleNormalToTargetDelta, fromTo) * middleNormal;

	// Rotate middle Joint to solve position
	Vector3f lowerDirection = solvePosition - middleJoint.getPosition();
	middleNormal = projectOnPlane(middleNormal.normalized(), lowerDirection.normalized());
	middleJoint.setRotationTowards(lowerDirection, middleNormal);

	// Rotate lower Joint to target rotation
	lowerJoint.setRotation(targetRotation);

	/*
	if (toe != nullptr) {

		Vector3f toePosition = toe->getGlobalPosition();

		if (toePosition.y() < toeCalibratedY) {

			Vector3f toToe = toePosition - lowerJoint.getPosition();
			Vector3f toToeAbove = Vector3f(toePosition.x(), toeCalibratedY,toePosition.z()) - lowerJoint.getPosition();

			DebugPos2 = Vector3f(toePosition.x(), toeCalibratedY, toePosition.z());

			Quaternionf toToeRotation = fromToRotation(toToe, toToeAbove);
			toe->setGlobalRotation(toToeRotation * toe->getGlobalRotation());
		}
	}
	*/
}