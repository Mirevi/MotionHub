#include "IKSolverArm.h"

IKSolverArm::IKSolverArm() {

}

IKSolverArm::IKSolverArm(HierarchicJoint* shoulder, HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower)
	// Base Constructor
	: IKSolverLeg(upper, middle, lower) {

	// Create IKJoint object from shoulder
	shoulderJoint = IKJoint(shoulder);

	// Add pointer for IKJoint in vector
	// TODO: FABRIK nur mit 2 Joints -> Schulter durch vitruelle esetzen
	// joints.insert(joints.begin(), &shoulderJoint);
}

void IKSolverArm::setChest(HierarchicJoint* joint) {

	chest = joint;
}

void IKSolverArm::init() {

	IKSolverLeg::init();

	// Initiate joint
	Vector3f shoulderToUpper = upperJoint.getPosition() - shoulderJoint.getPosition();
	shoulderJoint.init(shoulderToUpper, normal);

	// Save shoulder to hand distance
	defaultShoulderHandDistance = (lowerJoint.getLocalPosition() - shoulderJoint.getPosition()).norm();

	// TODO: Achsen bestimmen
	// - Handflaeche zu Mittelfinger
	// - Handflaeche zu Daumen

	// TODO: isLeftArm needed?
	// Cache if this is the LeftArm Solver
	isLeftArm = upperJoint.joint->getJointName() == Joint::ARM_L;

	// LeftArm: Swap shoulder angles
	if(isLeftArm) {
		float tempAngle = shoulderBackYAngle;
		shoulderBackYAngle = shoulderForwardYAngle;
		shoulderForwardYAngle = tempAngle;

		tempAngle = shoulderUpZAngle;
		shoulderUpZAngle = shoulderDownMaxAngle;
		shoulderDownMaxAngle = tempAngle;
	}

	saveDefaultState();
}

void IKSolverArm::saveDefaultState() {

	// Save default state for shoulder
	shoulderJoint.saveDefaultState();

	shoulderDefaultRotation = shoulderJoint.getRotation();

	// Call save on base class
	IKSolverLeg::saveDefaultState();
}

void IKSolverArm::loadDefaultState() {

	// Save default state for shoulder
	shoulderJoint.loadDefaultState();

	// Call load on base class
	IKSolverLeg::loadDefaultState();
}


void IKSolverArm::solve(Vector3f position, Quaternionf rotation) {

	IKSolverLeg::solve(position, rotation);
}

void IKSolverArm::solve() {

	// TODO: Solve Elbow by Tracker
	if (solveShoulder) {

		// Read shoulder positon & relative shoulder rotation
		Vector3f shoulderPosition = shoulderJoint.getPosition();
		Quaternionf shoulderRotation = shoulderJoint.getRotation() * shoulderDefaultRotation.inverse();
		
		// Calculate vectors from shoulder to current elbow & new elbow (from last frame)
		Vector3f toMiddle = middleJoint.getPosition() - shoulderPosition;
		Vector3f toLastMiddle = lastMiddlePosition - shoulderPosition;

		// Calculate rotation from current elbow to new elbow position 
		Quaternionf solvedRotation = fromToRotation(toMiddle, toLastMiddle);

		// Extract local shoulder y- & z-axis
		Vector3f yAxis = shoulderRotation * Vector3f(0, 1, 0);
		Vector3f zAxis = shoulderRotation * Vector3f(0, 0, 1);

		// Decompose y rotation & calculate y angle
		Quaternionf yRotation = decomposeTwist(solvedRotation, yAxis);
		float yAngle = signedAngle(zAxis, yRotation * zAxis, yAxis);

		// Decompose z rotation & calculate z angle
		Quaternionf zRotation = decomposeTwist(solvedRotation, zAxis);
		float zAngle = signedAngle(yAxis, zRotation * yAxis, zAxis);

		Vector3f shoulderToTarget = targetPosition - shoulderPosition;
		Vector3f shoulderToTargetOnUp = projectOnPlane(shoulderToTarget, chest->getUp());
		Vector3f shoulderToTargetOnForward = projectOnPlane(shoulderToTarget, chest->getForward());

		/*std::string arm = "RightArm";
		if (isLeftArm) arm = "LeftArm";
		Console::log(arm + " # " + toString(yAngle) + " " + toString(zAngle));*/

		// Get y max angle from config
		float yMaxAngle = shoulderForwardYAngle;
		// Is y angle negative? -> position is behind shoulder
		if (yAngle < 0) {
			yMaxAngle = shoulderBackYAngle;
		}

		// Add extra allowed angle by shoulder-target distance
		float handYAngle = mapClamp(shoulderToTargetOnUp.norm(), 0, defaultShoulderHandDistance, 0, shoulderHandMaxAngle);
		yMaxAngle = fabs(yMaxAngle) + handYAngle;

		// Clamp y rotation by shoulder-elbow angle & shoulder-hand distance
		float yDelta = mapClamp(fabs(yAngle), 0, 90, 0, yMaxAngle);
		yRotation = rotateTowards(Quaternionf::Identity(), yRotation, yDelta);

		// Get z max angle from config
		float zMaxAngle = shoulderDownMaxAngle;
		// Is z angle negative? -> position is above shoulder
		if (zAngle < 0) {
			zMaxAngle = shoulderUpZAngle;
		}
		// Add extra allowed angle by shoulder-target distance

		float handZAngle = mapClamp(shoulderToTargetOnForward.norm(), 0, defaultShoulderHandDistance, 0, shoulderHandMaxAngle);
		zMaxAngle = fabs(zMaxAngle) + handZAngle;

		// Clamp z rotation by shoulder-elbow angle & shoulder-hand distance
		float zDelta = mapClamp(fabs(zAngle), 0, 90, 0, zMaxAngle);
		zRotation = rotateTowards(Quaternionf::Identity(), zRotation, zDelta);

		// Get direction to upper relative to shoulder
		Vector3f direction = upperJoint.getPosition() - shoulderPosition;
		// Rotate it by y- & z-rotation
		direction = (yRotation * zRotation) * direction; 

		// Set upper joint solved position
		upperJoint.setSolvedPosition(direction + shoulderPosition);
	}

	IKSolverLeg::solve();
}

void IKSolverArm::constraint() {

	// TODO: Arm constraint implementieren


	// Store solved positions of all 4 joints
	Vector3f shoulderPosition = shoulderJoint.getSolvedPosition();
	Vector3f upperPosition = upperJoint.getSolvedPosition();
	Vector3f middlePosition = middleJoint.getSolvedPosition();
	Vector3f lowerPosition = lowerJoint.getSolvedPosition();

	// Get direction the limb should point to
	Vector3f middleDirection = calculateMiddleDirection();


	// TODO: AUSKOMMENTIEREN
	IKSolverLeg::constraint();
}

void IKSolverArm::apply() {

	// TODO: Schulter apply

	// Rotate shoulder Joint
	if (solveShoulder) {
		Vector3f upperDirection = upperJoint.getSolvedPosition() - shoulderJoint.getPosition();
		shoulderJoint.setRotationTowards(upperDirection);
	}

	IKSolverLeg::apply();

	lastMiddlePosition = middleJoint.getPosition();
}

void IKSolverArm::untwist() {


	// TODO: Untwist neu schreiben
	//IKSolverLeg::untwist();
}
