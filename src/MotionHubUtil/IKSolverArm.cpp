#include "IKSolverArm.h"

IKSolverArm::IKSolverArm() {

}

IKSolverArm::IKSolverArm(HierarchicJoint* shoulder, HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower) 
	// Base Constructor
	: IKSolverLimb(upper, middle, lower) {

	// Create IKJoint object from shoulder
	shoulderJoint = IKJoint(shoulder);

	// Add pointer for IKJoint in vector
	joints.insert(joints.begin(), &shoulderJoint);
}

void IKSolverArm::init() {

	IKSolverLimb::init();

	// Initiate joint
	Vector3f shoulderToUpper = upperJoint.getPosition() - shoulderJoint.getPosition();
	shoulderJoint.init(shoulderToUpper, normal);

	//wristToPalmAxis = GuessWristToPalmAxis(lowerBone.transform, middleBone.transform);
	//palmToThumbAxis = GuessPalmToThumbAxis(lowerBone.transform, chest);

	saveDefaultState();
}

void IKSolverArm::saveDefaultState() {

	// Save default state for shoulder
	shoulderJoint.saveDefaultState();

	// Call save on base class
	IKSolverLimb::saveDefaultState();
}

void IKSolverArm::loadDefaultState() {

	// Save default state for shoulder
	shoulderJoint.loadDefaultState();

	// Call load on base class
	IKSolverLimb::loadDefaultState();
}


void IKSolverArm::solve(Vector3f position, Quaternionf rotation) {

	IKSolverLimb::solve(position, rotation);
}

Vector3f IKSolverArm::getStartPosition() {

	// TODO: Solve Shoulder implementieren

	if (solveShoulder) {
		//return targetArm.position;
		// TODO: Trackerposition
		return IKSolverLimb::getStartPosition();
	}
	else {
		return IKSolverLimb::getStartPosition();
	}
}

void IKSolverArm::solve() {

	if (solveShoulder) {
		upperJoint.setSolvedPosition(getStartPosition());
	}

	IKSolverLimb::solve();
}

void IKSolverArm::constraint() {

	// TODO: Arm constraint implementieren
}

void IKSolverArm::apply() {

	// Rotate shoulder Joint
	if (solveShoulder) {
		Vector3f upperDirection = upperJoint.getSolvedPosition() - shoulderJoint.getPosition();
		// TODO: Bend Normal?
		shoulderJoint.setRotationTowards(upperDirection);
	}

	IKSolverLimb::apply();
}

void IKSolverArm::untwist() {

	IKSolverLimb::untwist();
}
