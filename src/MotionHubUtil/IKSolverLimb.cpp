#include "IKSolverLimb.h"

IKSolverLimb::IKSolverLimb() {

}

IKSolverLimb::IKSolverLimb(HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower) {

	// Create IKJoint objects from given joints
	upperJoint = IKJoint(upper);
	middleJoint = IKJoint(middle);
	lowerJoint = IKJoint(lower);

	// Add pointers for all IKJoints in vector
	joints.push_back(&upperJoint);
	joints.push_back(&middleJoint);
	joints.push_back(&lowerJoint);
}

void IKSolverLimb::init() {

	// Store middle & lower position
	Vector3f middlePosition = middleJoint.getPosition();
	Vector3f lowerPosition = lowerJoint.getPosition();

	// Store chain direction vectors
	Vector3f upperToMiddle = middlePosition - upperJoint.getPosition();
	Vector3f middleToLower = lowerPosition - middlePosition;

	// Store normal (upwards on both direction vectors) for bending
	normal = upperToMiddle.cross(middleToLower);

	// Set target to current position & rotation
	targetPosition = lowerPosition;
	targetRotation = lowerJoint.getRotation();

	// Initiate joints
	upperJoint.init(upperToMiddle, normal);
	middleJoint.init(middleToLower, normal);

	saveDefaultState();
}

void IKSolverLimb::saveDefaultState() {

	// Save default state for all joints
	upperJoint.saveDefaultState();
	middleJoint.saveDefaultState();
	lowerJoint.saveDefaultState();

	// Save lower default rotation
	lowerDefaultRotation = lowerJoint.joint->getGlobalRotation();
}

void IKSolverLimb::loadDefaultState() {

	// Load default state for all joints
	upperJoint.loadDefaultState();
	middleJoint.loadDefaultState();
	lowerJoint.loadDefaultState();
}

void IKSolverLimb::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolver::solve(position, rotation);

	// Restore default state
	loadDefaultState();

	// Store current position in each joint
	for (IKJoint* joint : joints) {
		joint->saveSolvedPosition();
	}

	// Store normal (so it can be manipulated and restored)
	_normal = normal;

	// Solve IK
	solve();

	// Constraint solution
	constraint();

	// Apply solution to joints
	apply();

	// Untwist lower joint based target rotation
	untwist();

	// Restore normal
	normal = _normal;
}

Vector3f IKSolverLimb::getStartPosition() {

	return upperJoint.joint->getGlobalPosition();
}

void IKSolverLimb::solve() {

	// TODO: Fallunterscheidung mit und ohne Bend Goal
	//if (bend == nullptr) {

	Quaternionf rotation = targetRotation * lowerDefaultRotation.inverse();
	normal = Quaternionf::Identity().slerp(bendToTargetRotationWeight, rotation) * normal;

	//else {
	//}

	// normalize normal
	normal = normal.normalized();

	// store start position
	startPosition = getStartPosition();

	// init distance to target with "infinity"
	float lastDistance = FLT_MAX;

	// loop until max allowed iterations or a break condition is reached
	for (int iteration = 1; iteration <= iterations; iteration++) {

		// FABRIK algorithm
		forwardReach();
		backwardReach();

		// Calculate distance between target position & last joint position
		float distanceToTarget = distance(targetPosition, lowerJoint.getSolvedPosition());

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

void IKSolverLimb::constraint() {

	// TODO: bend Direction 

	// Store solved positions of all 3 joints
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f middlePosition = middleJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Store direction from upper joint to mid joint & bend from normal
	Vector3f targetDirection = targetPosition - upperPosition;
	Vector3f direction = targetDirection.cross(normal);

	if (lowerJoint.joint->getJointName() == Joint::FOOT_R) {
		// TODO: Testen!
		//direction += (bendDirection * 0.5f);
	}

	//if (DebugFloat1 > 0) {
	//	direction = bendDirection * DebugFloat1;
	//}

	// Get the rotation limb axis from upper to lower joint
	Vector3f limbAxis = (lowerPosition - upperPosition).normalized();

	// Store current direction
	Vector3f currentDirection = (middlePosition - upperPosition).normalized();

	// Ortho normalize both directions on limb axis
	orthoNormalize(limbAxis, currentDirection);
	orthoNormalize(limbAxis, direction);

	//TODO: bendDirNullNew

	// Calculate angle difference on limb axis
	float angle = signedAngle(currentDirection, direction, limbAxis);
	Quaternionf rotation = angleAxis(angle, limbAxis);

	// Rotate the middle bone using the angle
	Vector3f newMiddlePosition = rotation * (middlePosition - upperPosition) + upperPosition;
	middleJoint.setSolvedPosition(newMiddlePosition);
}

void IKSolverLimb::apply() {

	// TODO: Use Actual Position (IKSolverArm)

	// Store middle position 
	Vector3f middlePosition = middleJoint.getSolvedPosition();

	// Rotate upper Joint
	Vector3f middleDirection = middlePosition - upperJoint.getSolvedPosition();
	upperJoint.setRotationTowards(middleDirection, normal);

	// Rotate middle Joint
	Vector3f lowerDirection = targetPosition - middlePosition;
	middleJoint.setRotationTowards(lowerDirection);

	// TODO: preTwist(); ?

	// Rotate lower Joint
	lowerJoint.setRotation(targetRotation);
}

void IKSolverLimb::untwist() {

	// TODO: confident angle? 180° flips

	// TODO: Debug raus
	if (lowerJoint.joint->getJointName() == Joint::FOOT_R) {
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

		clearDebugLines();

		debugDrawLine(middlePosition, middlePosition + middleJoint.joint->getForward(), Vector3f(1.0f, 1.0f, 1.0f));

		// Store inverse middle rotation
		Quaternionf invMiddleRotation = middleRotation.inverse();

		// Store middle joint right, up & forward axis
		Vector3f middleRight = middleRotation * Vector3f(1, 0, 0);
		Vector3f middleUp = middleRotation * Vector3f(0, 1, 0);
		Vector3f middleForward = middleRotation * Vector3f(0, 0, 1);

		// Store rotational difference between lower & middle joint
		Quaternionf diff = lowerRotation * invMiddleRotation;

		// Decompose twist in relation to middle forward axis
		Quaternionf twist = DecomposeTwist(diff, middleForward);

		// TODO: UpperRotation anstatt upperTwist? 
		//Quaternionf middleTwist = upperTwist.slerp(middleUntwistWeight, twist);

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
		Quaternionf twist = DecomposeTwist(diff, upperUp);

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
