#include "IKSolverSpine.h"

IKSolverSpine::IKSolverSpine() {

}

IKSolverSpine::IKSolverSpine(HierarchicJoint* spine, HierarchicJoint* chest, HierarchicJoint* neck, HierarchicJoint* head) {

	// Create IKJoint objects from given joints
	this->spine = IKJoint(spine);
	this->chest = IKJoint(chest);
	this->neck = IKJoint(neck);
	this->head = IKJoint(head);
}

void IKSolverSpine::setHip(HierarchicJoint* joint) {

	// Store reference to hip joint
	hip = joint;
}

void IKSolverSpine::init() {

	// Init CCD joints
	ccdJoints.push_back(&chest);
	ccdJoints.push_back(&spine);

	// Init Twist joints
	twistJoints.push_back(&spine);
	twistWeights.push_back(0.5f);

	twistJoints.push_back(&chest);
	twistWeights.push_back(0.5f);

	twistJoints.push_back(&neck);
	twistWeights.push_back(1.0f);

	// Save default state for joints
	saveDefaultState();
}

void IKSolverSpine::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolver::solve(position, rotation);

	// Restore default state
	loadDefaultState();
}

void IKSolverSpine::saveDefaultState() {
	//headLocalPosition = head.getLocalPosition();
	//headLocalRotation = head.getLocalRotation();
}

void IKSolverSpine::loadDefaultState() {
	//head.setLocalPosition(headLocalPosition);
	//head.setLocalRotation(headLocalRotation);
}
