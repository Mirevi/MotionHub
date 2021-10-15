#include "IKSolverHip.h"


IKSolverHip::IKSolverHip(HierarchicJoint* joint) {

	hip = joint;
}

void IKSolverHip::setLeftLeg(HierarchicJoint* upper, HierarchicJoint* foot) {

	leftUpperLeg = upper;
	leftFoot = foot;
}

void IKSolverHip::setRightLeg(HierarchicJoint* upper, HierarchicJoint* foot) {

	rightUpperLeg = upper;
	rightFoot = foot;
}

void IKSolverHip::init() {

	refresh(true);
}

void IKSolverHip::refresh(bool overrideDefaults) {

	// TODO: sqr norm?

	Vector3f leftUpperPosition = leftUpperLeg->getGlobalPosition();
	Vector3f rightUpperPosition = rightUpperLeg->getGlobalPosition();

	if (overrideDefaults) {
		// LeftLeg: Store length from upper to foot
		Vector3f leftFootPosition = leftFoot->getGlobalPosition();
		leftLegReach = (leftFootPosition - leftUpperPosition).norm();

		// RightLeg: Store length from upper to foot
		Vector3f rightFootPosition = rightFoot->getGlobalPosition();
		rightLegReach = (rightFootPosition - rightUpperPosition).norm();		
	}

	// LeftLeg: Store length from each joint
	Vector3f leftMiddlePosition = leftFoot->getParent()->getGlobalPosition();
	leftLegLength = (leftMiddlePosition - leftUpperPosition).norm()
		+ (leftFootPosition - leftMiddlePosition).norm();

	// RightLeg: Store length from each joint
	Vector3f rightMiddlePosition = rightFoot->getParent()->getGlobalPosition();
	rightLegLength = (rightMiddlePosition - rightUpperPosition).norm()
		+ (rightFootPosition - rightMiddlePosition).norm();

	// Reset hint
	this->leftFootPosition = Vector3f::Zero();
	this->rightFootPosition = Vector3f::Zero();
	hasHint = false;
}

void IKSolverHip::solve(Vector3f position, Quaternionf rotation) {

	// Apply target position & rotation to hip
	hip->setGlobalPosition(position);
	hip->setGlobalRotation(rotation);

	// Skip Hip adjustment in calibration mode or without hint
	if (isCalibrating || !hasHint) {
		return;
	}

	// TODO: Reach oder Lenght?

	// Init hip offset with zero
	Vector3f hipOffset = Vector3f::Zero();

	// LeftLeg: Store upper position & calculate vector to foot
	Vector3f leftUpperPosition = leftUpperLeg->getGlobalPosition();
	Vector3f leftUpperToFoot = leftFootPosition - leftUpperPosition;

	bool isLeftInReach = true;
	if (leftUpperToFoot.norm() >= leftLegReach) {
		isLeftInReach = false;

		Vector3f leftReachablePosition = leftUpperPosition + leftUpperToFoot.normalized() * leftLegReach;

		hipOffset += leftFootPosition - leftReachablePosition;

		//Console::logWarning("L");
	}

	// RightLeg: Store upper position & calculate vector to foot
	Vector3f rightUpperPosition = rightUpperLeg->getGlobalPosition();
	Vector3f rightUpperToFoot = rightFootPosition - rightUpperPosition;

	//bool isRightInReach = true;
	if (rightUpperToFoot.norm() >= rightLegReach) {
		//isRightInReach = false;

		Vector3f rightReachablePosition = rightUpperPosition + rightUpperToFoot.normalized() * rightLegReach;

		hipOffset += rightFootPosition - rightReachablePosition;

		if (!isLeftInReach) {
			hipOffset = hipOffset * 0.5f;
		}

		//Console::logWarning("R");
	}

	if (!(GetAsyncKeyState(VK_LMENU) & 0x8000)) {
		hip->setGlobalPosition(position + hipOffset);
	}
}

void IKSolverHip::hint(Vector3f leftFoot, Vector3f rightFoot) {

	leftFootPosition = leftFoot;
	rightFootPosition = rightFoot;

	hasHint = true;
}

void IKSolverHip::saveDefaultState() {

}

void IKSolverHip::loadDefaultState() {

}