#include "IKSolverHip.h"


IKSolverHip::IKSolverHip(HierarchicJoint* joint) {

	hip = IKJoint(joint);
}

void IKSolverHip::init() {

}

void IKSolverHip::solve(Vector3f position, Quaternionf rotation) {

	hip.joint->setGlobalPosition(position);
	hip.joint->setGlobalRotation(rotation);
}

void IKSolverHip::saveDefaultState() {

}

void IKSolverHip::loadDefaultState() {

}