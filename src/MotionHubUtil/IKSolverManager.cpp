#include "IKSolverManager.h"

IKSolverManager::IKSolverManager() {

}

IKSolverManager::~IKSolverManager() {

	if (ikSolverHip != nullptr) delete ikSolverHip;
	if (ikSolverSpine != nullptr) delete ikSolverSpine;
	if (ikSolverLeftLeg != nullptr) delete ikSolverLeftLeg;
	if (ikSolverRightLeg != nullptr) delete ikSolverRightLeg;
	if (ikSolverLeftArm != nullptr) delete ikSolverLeftArm;
	if (ikSolverRightArm != nullptr) delete ikSolverRightArm;

	if (hierarchicSkeleton != nullptr) delete hierarchicSkeleton;
	if (skeleton != nullptr) delete skeleton;
}

void IKSolverManager::initSkeleton(Skeleton otherSkeleton, int sid) {

	initSkeleton(sid);

	hierarchicSkeleton->init(otherSkeleton);
}

void IKSolverManager::initSkeleton(int sid) {

	skeleton = new Skeleton(sid);
	hierarchicSkeleton = new HierarchicSkeleton(sid);

	initialized = true;
}

void IKSolverManager::initIKSolvers(HierarchicSkeleton* hierarchicSkeleton) {

	// Init Hip IKSolver
	if (ikSolverHip != nullptr) delete ikSolverHip;
	ikSolverHip = new IKSolverHip(&hierarchicSkeleton->hips);
	ikSolverHip->setLeftLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftFoot);
	ikSolverHip->setRightLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightFoot);
	ikSolverHip->init();

	// Init Spine IKSolver
	if (ikSolverSpine != nullptr) delete ikSolverSpine;
	ikSolverSpine = new IKSolverSpine(&hierarchicSkeleton->spine, &hierarchicSkeleton->chest, &hierarchicSkeleton->neck, &hierarchicSkeleton->head);
	ikSolverSpine->setHip(&hierarchicSkeleton->hips);
	ikSolverSpine->init();

	// Init LeftLeg IKSolver
	if (ikSolverLeftLeg != nullptr) delete ikSolverLeftLeg;
	ikSolverLeftLeg = new IKSolverLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftLeg, &hierarchicSkeleton->leftFoot);
	//ikSolverLeftLeg->seToe(&hierarchicSkeleton->leftToe);
	ikSolverLeftLeg->init();

	// Init RightLeg IKSolver
	if (ikSolverRightLeg != nullptr) delete ikSolverRightLeg;
	ikSolverRightLeg = new IKSolverLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightLeg, &hierarchicSkeleton->rightFoot);
	//ikSolverRightLeg->seToe(&hierarchicSkeleton->rightToe);
	ikSolverRightLeg->init();

	// Init LeftArm IKSolver
	if (ikSolverLeftArm != nullptr) delete ikSolverLeftArm;
	ikSolverLeftArm = new IKSolverArm(&hierarchicSkeleton->leftShoulder, &hierarchicSkeleton->leftArm, &hierarchicSkeleton->leftForeArm, &hierarchicSkeleton->leftHand);
	ikSolverLeftArm->setChest(&hierarchicSkeleton->chest);
	ikSolverLeftArm->init();

	// Init RightArm IKSolver
	if (ikSolverRightArm != nullptr) delete ikSolverRightArm;
	ikSolverRightArm = new IKSolverArm(&hierarchicSkeleton->rightShoulder, &hierarchicSkeleton->rightArm, &hierarchicSkeleton->rightForeArm, &hierarchicSkeleton->rightHand);
	ikSolverRightArm->setChest(&hierarchicSkeleton->chest);
	ikSolverRightArm->init();

	// Solve in T-Pose
	ikSolverHip->solve(hierarchicSkeleton->hips.getGlobalPosition(), hierarchicSkeleton->hips.getGlobalRotation());
	ikSolverSpine->solve(hierarchicSkeleton->head.getGlobalPosition(), hierarchicSkeleton->head.getGlobalRotation());
	ikSolverLeftLeg->solve(hierarchicSkeleton->leftFoot.getGlobalPosition(), hierarchicSkeleton->leftFoot.getGlobalRotation());
	ikSolverRightLeg->solve(hierarchicSkeleton->rightFoot.getGlobalPosition(), hierarchicSkeleton->rightFoot.getGlobalRotation());
	ikSolverLeftArm->solve(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->leftHand.getGlobalRotation());
	ikSolverRightArm->solve(hierarchicSkeleton->rightHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalRotation());
}

void IKSolverManager::initIKSolvers() {

	initIKSolvers(hierarchicSkeleton);
}

void IKSolverManager::enableDirectMode() {

	ikSolverHip->enableDirectMode();
	ikSolverSpine->enableDirectMode();
	ikSolverLeftLeg->enableDirectMode();
	ikSolverRightLeg->enableDirectMode();
	ikSolverLeftArm->enableDirectMode();
	ikSolverRightArm->enableDirectMode();
}

void IKSolverManager::refreshIKSolvers(bool overrideDefault) {

	if (ikSolverHip != nullptr) {
		ikSolverHip->refresh(overrideDefault);
	}

	if (ikSolverLeftLeg != nullptr) {
		ikSolverLeftLeg->refresh(overrideDefault);
	}

	if (ikSolverRightLeg != nullptr) {
		ikSolverRightLeg->refresh(overrideDefault);
	}

	if (ikSolverLeftArm != nullptr) {
		ikSolverLeftArm->refresh(overrideDefault);
	}

	if (ikSolverRightArm != nullptr) {
		ikSolverRightArm->refresh(overrideDefault);
	}

	if (hierarchicSkeleton != nullptr) {
		hierarchicSkeleton->invalidateGlobals();
	}
}
