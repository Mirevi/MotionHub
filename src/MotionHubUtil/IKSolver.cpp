#include "IKSolver.h"

IKSolver::IKSolver() {

}

IKSolver::~IKSolver() {

}

void IKSolver::solve(Vector3f position, Quaternionf rotation) {
	targetPosition = position;
	targetRotation = rotation;
}