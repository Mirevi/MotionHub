#include "HierarchicSkeleton.h"
// TODO: Debug: Console entfernen
#include "Console.h"


HierarchicSkeleton::HierarchicSkeleton(int sid) {
	skeleton = Skeleton(sid);

	scale = Vector3f::Ones();
}

void HierarchicSkeleton::init() {

	// TODO: Gröbere/Genauere Position und/oder Rotation?
	joints.clear();

	// Hips
	hips = HierarchicJoint(Vector3f(0, 1, 0), Quaternionf::Identity(), scale);
	addJoint(&hips, Joint::JointNames::HIPS);

	// LeftUpLeg
	//leftUpLeg = HierarchicJoint(Vector3f(-0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(180, 180, 0)));
	leftUpLeg = HierarchicJoint(Vector3f(0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftUpLeg.setParent(&hips);
	addJoint(&leftUpLeg, Joint::JointNames::UPLEG_L);

	// RightUpLeg
	//rightUpLeg = HierarchicJoint(Vector3f(0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(0, 0, 180)));
	rightUpLeg = HierarchicJoint(Vector3f(-0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightUpLeg.setParent(&hips);
	addJoint(&rightUpLeg, Joint::JointNames::UPLEG_R);

	// LeftLeg
	//leftLeg = HierarchicJoint(Vector3f(0, 0.428f, 0), eulerToQuaternion(Vector3f(-1, 0, 0)));
	leftLeg = HierarchicJoint(Vector3f(0, -0.428f, 0), eulerToQuaternion(Vector3f(1, 0, 0)), scale);
	leftLeg.setParent(&leftUpLeg);
	addJoint(&leftLeg, Joint::JointNames::LEG_L);

	// RightLeg
	//rightLeg = HierarchicJoint(Vector3f(0, 0.428f, 0), eulerToQuaternion(Vector3f(-1, 0, 0)));
	rightLeg = HierarchicJoint(Vector3f(0, -0.428f, 0), eulerToQuaternion(Vector3f(1, 0, 0)), scale);
	rightLeg.setParent(&rightUpLeg);
	addJoint(&rightLeg, Joint::JointNames::LEG_R);

	// LeftFoot
	//leftFoot = HierarchicJoint(Vector3f(0, 0.421f, 0), eulerToQuaternion(Vector3f(57.75f, 0, 0)));
	leftFoot = HierarchicJoint(Vector3f(0, -0.421f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftFoot.setParent(&leftLeg);
	addJoint(&leftFoot, Joint::JointNames::FOOT_L);

	// RightFoot
	//rightFoot = HierarchicJoint(Vector3f(0, 0.421f, 0), eulerToQuaternion(Vector3f(57.75f, 0, 0)));
	rightFoot = HierarchicJoint(Vector3f(0, -0.421f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightFoot.setParent(&rightLeg);
	addJoint(&rightFoot, Joint::JointNames::FOOT_R);

	// LeftToe
	//leftToe = HierarchicJoint(Vector3f(0, 0.162f, 0), eulerToQuaternion(Vector3f(33, 0, 0)));
	leftToe = HierarchicJoint(Vector3f(-0.011f, -0.091f, 0.134f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftToe.setParent(&leftFoot);
	addJoint(&leftToe, Joint::JointNames::TOE_L);

	// RightToe
	//rightToe = HierarchicJoint(Vector3f(0, 0.162f, 0), eulerToQuaternion(Vector3f(33, 0, 0)));
	rightToe = HierarchicJoint(Vector3f(0.011f, -0.091f, 0.134f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightToe.setParent(&rightFoot);
	addJoint(&rightToe, Joint::JointNames::TOE_R);


	// Spine
	spine = HierarchicJoint(Vector3f(0, 0.097f, -0.01f), eulerToQuaternion(Vector3f(-6.925f, 0, 0)), scale);
	spine.setParent(&hips);
	addJoint(&spine, Joint::JointNames::SPINE);

	// Chest
	chest = HierarchicJoint(Vector3f(0, 0.245f, 0), Quaternionf::Identity(), scale);
	chest.setParent(&spine);
	addJoint(&chest, Joint::JointNames::CHEST);

	// Neck
	neck = HierarchicJoint(Vector3f(0, 0.157f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	neck.setParent(&chest);
	addJoint(&neck, Joint::JointNames::NECK);

	// Head
	head = HierarchicJoint(Vector3f(0, 0.067f, 0.019f), Quaternionf::Identity(), scale);
	head.setParent(&neck);
	addJoint(&head, Joint::JointNames::HEAD);

	// LeftShoulder
	//leftShoulder = HierarchicJoint(Vector3f(-0.061f, 0.13f, 0), eulerToQuaternion(Vector3f(105, 0, -92)));
	//leftShoulder = HierarchicJoint(Vector3f(-0.061f, 0.13f, 0), eulerToQuaternion(Vector3f(105, -10, -92)));
	leftShoulder = HierarchicJoint(Vector3f(0.061f, 0.129f, -0.0175f), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	leftShoulder.setParent(&chest);
	addJoint(&leftShoulder, Joint::JointNames::SHOULDER_L);

	// RightShoulder
	//rightShoulder = HierarchicJoint(Vector3f(0.061f, 0.13f, 0), eulerToQuaternion(Vector3f(105, 0, 92)));
	//rightShoulder = HierarchicJoint(Vector3f(0.061f, 0.13f, 0), eulerToQuaternion(Vector3f(105, 10, 92)));
	rightShoulder = HierarchicJoint(Vector3f(-0.061f, 0.129f, -0.0175f), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	rightShoulder.setParent(&chest);
	addJoint(&rightShoulder, Joint::JointNames::SHOULDER_R);

	// LeftArm
	//leftArm = HierarchicJoint(Vector3f(0, 0.13f, 0), eulerToQuaternion(Vector3f(-15, 0, -2)));
	leftArm = HierarchicJoint(Vector3f(0.126f, -0.0334f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftArm.setParent(&leftShoulder);
	addJoint(&leftArm, Joint::JointNames::ARM_L);

	// RightArm
	//rightArm = HierarchicJoint(Vector3f(0, 0.13f, 0), eulerToQuaternion(Vector3f(-15, 0, 2)));
	rightArm = HierarchicJoint(Vector3f(-0.126f, -0.0334f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightArm.setParent(&rightShoulder);
	addJoint(&rightArm, Joint::JointNames::ARM_R);

	// LeftForeArm
	//leftForeArm = HierarchicJoint(Vector3f(0, 0.256f, 0), eulerToQuaternion(Vector3f(0, 0, -1)));
	leftForeArm = HierarchicJoint(Vector3f(0.256f, 0, 0), eulerToQuaternion(Vector3f(0, -1, 0)), scale);
	leftForeArm.setParent(&leftArm);
	addJoint(&leftForeArm, Joint::JointNames::FOREARM_L);

	// RightForeArm
	//rightForeArm = HierarchicJoint(Vector3f(0, 0.256f, 0), eulerToQuaternion(Vector3f(0, 0, 1)));
	rightForeArm = HierarchicJoint(Vector3f(-0.256f, 0, 0), eulerToQuaternion(Vector3f(0, 1, 0)), scale);
	rightForeArm.setParent(&rightArm);
	addJoint(&rightForeArm, Joint::JointNames::FOREARM_R);

	// LeftHand
	//leftHand = HierarchicJoint(Vector3f(0, 0.234f, 0), eulerToQuaternion(Vector3f(1, -5, 4.5f)));
	leftHand = HierarchicJoint(Vector3f(0.234f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftHand.setParent(&leftForeArm);
	addJoint(&leftHand, Joint::JointNames::HAND_L);

	// RightHand
	//rightHand = HierarchicJoint(Vector3f(0, 0.234f, 0), eulerToQuaternion(Vector3f(1, 5, -4.5f)));
	rightHand = HierarchicJoint(Vector3f(-0.234f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightHand.setParent(&rightForeArm);
	addJoint(&rightHand, Joint::JointNames::HAND_R);


	// Convert to Unity Coordinate System
	//Quaternionf rotation = hips.getGlobalRotation();
	//rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());
	//hips.setGlobalRotation(rotation);

	// TODO: Update all children?
	invalidateGlobals();
}

void HierarchicSkeleton::reset() {

	//hips.setScale(Vector3f::Ones());
	scale = Vector3f::Ones();

	init();

	invalidateGlobals();
}

void HierarchicSkeleton::invalidateGlobals() {

	for (auto& joint : joints) {
		joint->invalidateGlobal();
	}
}

void HierarchicSkeleton::insert(Skeleton* currSkeleton) {

	//auto t1 = std::chrono::high_resolution_clock::now();

	// Loop over all Joints and insert into Skeleton
	for (auto& joint : joints) {

		// Conversion from OptiTrack Tracker:
		// rot = Quaternionf(rbData.qw, rbData.qx, -rbData.qy, -rbData.qz);
		// rot = Quaternionf(rot.y(), rot.z(), rot.w(), rot.x());
		// simplified: (-y, -z, w, x)

		Quaternionf rotation = joint->getGlobalRotation();
		rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());

		auto jointPointer = joint->jointPointer;
		jointPointer->setPosition(joint->getGlobalPosition4());
		jointPointer->setRotation(rotation);
		//joint->jointPointer->setPosition(joint->getGlobalPosition4());
		//joint->jointPointer->setRotation(rotation);

		//currSkeleton->m_joints.insert({ joint->getJointName(), joint->toJoint(rotation) });
		//currSkeleton->m_joints.insert({ joint->getJointName(), joint->toJoint() });
	}

	/*auto t2 = std::chrono::high_resolution_clock::now();
	auto ms_int2 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

	Console::logWarning(toString(ms_int2.count()));*/
}

void HierarchicSkeleton::setScale(Vector3f scale) {
	this->scale = scale;
}

Vector3f HierarchicSkeleton::getScale() {
	return scale;
}

void HierarchicSkeleton::addJoint(HierarchicJoint* joint, Joint::JointNames jointName) {

	// Set joint
	joint->setJointName(jointName);

	// Set pointer to internal Skeleton.joint
	Joint* jointPointer = &skeleton.m_joints[jointName];
	(*jointPointer) = Joint(Vector4f::Zero(), Quaternionf::Identity(), Joint::MEDIUM);
	joint->jointPointer = jointPointer;

	// Insert to joints vector
	joints.push_back(joint);
}

HierarchicJoint* HierarchicSkeleton::getJoint(Joint::JointNames jointName) {

	for (HierarchicJoint* joint : joints) {
		if (joint->getJointName() == jointName) {
			return joint;
		}
	}

	return nullptr;
}
