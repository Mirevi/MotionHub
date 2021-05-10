#include "IKSolverLimb.h"

IKSolverLimb::IKSolverLimb() {

}

void IKSolverLimb::saveDefaultState() {

}

void IKSolverLimb::loadDefaultState() {

}

void IKSolverLimb::init() {

}

void IKSolverLimb::solve(Vector3f position, Quaternionf rotation) {

	IKSolver::solve(position, rotation);

	// Restore Default State
	loadDefaultState();

}

Vector3f IKSolverLimb::getStartPosition() {
	return Vector3f::Zero();
}

void IKSolverLimb::solve() {

}

void IKSolverLimb::constraint() {

}

void IKSolverLimb::apply() {

}

void IKSolverLimb::untwist() {

	
}
