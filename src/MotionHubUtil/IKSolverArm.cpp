#include "IKSolverArm.h"

IKSolverArm::IKSolverArm() {

}

IKSolverArm::IKSolverArm(HierarchicJoint* shoulder, HierarchicJoint* upper, HierarchicJoint* middle, HierarchicJoint* lower) 
	// Base Constructor
	: IKSolverLeg(upper, middle, lower) {

	// Create IKJoint object from shoulder
	shoulderJoint = IKJoint(shoulder);

	// Add pointer for IKJoint in vector
	joints.insert(joints.begin(), &shoulderJoint);
}

void IKSolverArm::init() {

	IKSolverLeg::init();

	// Initiate joint
	Vector3f shoulderToUpper = upperJoint.getPosition() - shoulderJoint.getPosition();
	shoulderJoint.init(shoulderToUpper, normal);

	// TODO: Achsen bestimmen
	// - Handflaeche zu Mittelfinger
	// - Handflaeche zu Daumen

	saveDefaultState();
}

void IKSolverArm::saveDefaultState() {

	// Save default state for shoulder
	shoulderJoint.saveDefaultState();

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

Vector3f IKSolverArm::getStartPosition() {

	// TODO: Solve Shoulder implementieren

	if (solveShoulder) {
		//return targetArm.position;
		// TODO: Trackerposition
		return IKSolverLeg::getStartPosition();
	}
	else {
		return IKSolverLeg::getStartPosition();
	}
}

void IKSolverArm::solve() {

	if (solveShoulder) {
		upperJoint.setSolvedPosition(getStartPosition());
	}

	IKSolverLeg::solve();
}

void IKSolverArm::constraint() {

	// TODO: Arm constraint implementieren
}

void IKSolverArm::apply() {

	// TODO: Schulter apply

	// Rotate shoulder Joint
	if (solveShoulder) {
		Vector3f upperDirection = upperJoint.getSolvedPosition() - shoulderJoint.getPosition();
		// TODO: Bend Normal?
		shoulderJoint.setRotationTowards(upperDirection);
	}

	IKSolverLeg::apply();
}

void IKSolverArm::untwist() {

	IKSolverLeg::untwist();
}
