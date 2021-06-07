#include "IKSolverSpine.h"

IKSolverSpine::IKSolverSpine() {

}

void IKSolverSpine::init() {

	// TODO: Im Tracker aufrufen?
	// Save default state
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
