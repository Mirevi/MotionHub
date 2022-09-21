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

	//leftToeEnd = HierarchicJoint(Vector3f(0, 0, 0.06f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	//leftToeEnd.setParent(&leftToe);

	// RightToe
	//rightToe = HierarchicJoint(Vector3f(0, 0.162f, 0), eulerToQuaternion(Vector3f(33, 0, 0)));
	rightToe = HierarchicJoint(Vector3f(0.011f, -0.091f, 0.134f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightToe.setParent(&rightFoot);
	addJoint(&rightToe, Joint::JointNames::TOE_R);

	//rightToeEnd = HierarchicJoint(Vector3f(0, 0, 0.06f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	//rightToeEnd.setParent(&rightToe);

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

void HierarchicSkeleton::initXsens() {

	useXsens = true;

	joints.clear();

	// Hips
	// 0.984
	hips = HierarchicJoint(Vector3f(0, 0.984f, 0), Quaternionf::Identity(), scale);
	addJoint(&hips, Joint::JointNames::HIPS);

	// LeftUpLeg
	// 0.093007, 0.009817, 0.023847
	// 0.096516
	//leftUpLeg = HierarchicJoint(Vector3f(0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	//leftUpLeg = HierarchicJoint(Vector3f(0.094f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftUpLeg = HierarchicJoint(Vector3f(0.094f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftUpLeg.setParent(&hips);
	addJoint(&leftUpLeg, Joint::JointNames::UPLEG_L);

	// RightUpLeg
	// -0.093000, -0.009308, -0.024029
	// 0.096504
	//rightUpLeg = HierarchicJoint(Vector3f(-0.08f, -0.054f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	//rightUpLeg = HierarchicJoint(Vector3f(-0.094f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightUpLeg = HierarchicJoint(Vector3f(-0.094f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightUpLeg.setParent(&hips);
	addJoint(&rightUpLeg, Joint::JointNames::UPLEG_R);

	// LeftLeg
	// 0.029772, -0.522304, 0.020698
	// 0.523561
	//leftLeg = HierarchicJoint(Vector3f(0, -0.428f, 0), eulerToQuaternion(Vector3f(0.1f, 0, 0)), scale);
	leftLeg = HierarchicJoint(Vector3f(0, -0.524f, 0), eulerToQuaternion(Vector3f(1, 0, 0)), scale);
	leftLeg.setParent(&leftUpLeg);
	addJoint(&leftLeg, Joint::JointNames::LEG_L);

	// RightLeg
	// 0.090926, -0.514023, 0.028831
	// 0.522798
	//rightLeg = HierarchicJoint(Vector3f(0, -0.428f, 0), eulerToQuaternion(Vector3f(0.1f, 0, 0)), scale);
	rightLeg = HierarchicJoint(Vector3f(0, -0.524f, 0), eulerToQuaternion(Vector3f(1, 0, 0)), scale);
	rightLeg.setParent(&rightUpLeg);
	addJoint(&rightLeg, Joint::JointNames::LEG_R);

	// LeftFoot
	// 0.023574, -0.341522, -0.039665
	// 0.344625
	//leftFoot = HierarchicJoint(Vector3f(0, -0.421f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftFoot = HierarchicJoint(Vector3f(0, -0.346f, 0.001f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftFoot.setParent(&leftLeg);
	addJoint(&leftFoot, Joint::JointNames::FOOT_L);

	// RightFoot
	// -0.008369, -0.342867, -0.050823
	// 0.346714
	//rightFoot = HierarchicJoint(Vector3f(0, -0.421f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightFoot = HierarchicJoint(Vector3f(0, -0.346f, 0.001f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightFoot.setParent(&rightLeg);
	addJoint(&rightFoot, Joint::JointNames::FOOT_R);

	// LeftToe
	// -0.070925, -0.102424, 0.167665
	// 0.208884
	//leftToe = HierarchicJoint(Vector3f(-0.011f, -0.091f, 0.134f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftToe = HierarchicJoint(Vector3f(0, -0.089f, 0.189f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftToe.setParent(&leftFoot);
	addJoint(&leftToe, Joint::JointNames::TOE_L);

	// RightToe
	// -0.117325, -0.120187, 0.117347
	// 0.204891
	//rightToe = HierarchicJoint(Vector3f(0.011f, -0.091f, 0.134f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightToe = HierarchicJoint(Vector3f(0, -0.089f, 0.189f), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightToe.setParent(&rightFoot);
	addJoint(&rightToe, Joint::JointNames::TOE_R);

	// Spine
	// -0.001902, 0.105405, -0.043724
	// 0.114130
	//spine = HierarchicJoint(Vector3f(0, 0.097f, -0.01f), eulerToQuaternion(Vector3f(-6.925f, 0, 0)), scale);
	spine = HierarchicJoint(Vector3f(0, 0.113f, -0.013f), eulerToQuaternion(Vector3f(-6.925f, 0, 0)), scale);
	spine.setParent(&hips);
	addJoint(&spine, Joint::JointNames::SPINE);

	// Chest
	// 0.003426, 0.204152, -0.014417
	// 0.204689
	//chest = HierarchicJoint(Vector3f(0, 0.245f, 0), Quaternionf::Identity(), scale);
	// 0.107
	// 0.098
	chest = HierarchicJoint(Vector3f(0, 0.205f, 0), Quaternionf::Identity(), scale);
	chest.setParent(&spine);
	addJoint(&chest, Joint::JointNames::CHEST);

	// Neck
	// 0.018942, 0.229307, 0.027402
	// 0.231714
	//neck = HierarchicJoint(Vector3f(0, 0.157f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	// 0.098
	// 0.136
	neck = HierarchicJoint(Vector3f(0, 0.234f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	neck.setParent(&chest);
	addJoint(&neck, Joint::JointNames::NECK);

	// Head
	// 0.008421, 0.060716, 0.061935
	// 0.087139
	//head = HierarchicJoint(Vector3f(0, 0.067f, 0.019f), Quaternionf::Identity(), scale);
	head = HierarchicJoint(Vector3f(0, 0.087f, 0.001f), Quaternionf::Identity(), scale);
	head.setParent(&neck);
	addJoint(&head, Joint::JointNames::HEAD);

	// LeftShoulder
	// 0.042378, 0.169018, 0.014114
	// 0.174821
	//leftShoulder = HierarchicJoint(Vector3f(0.061f, 0.129f, -0.0175f), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	//leftShoulder = HierarchicJoint(Vector3f(0.029f, 0.116f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	leftShoulder = HierarchicJoint(Vector3f(0.029f, 0.174f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	leftShoulder.setParent(&chest);
	addJoint(&leftShoulder, Joint::JointNames::SHOULDER_L);

	// RightShoulder
	// -0.015655, 0.173515, 0.018255
	// 0.175174
	//rightShoulder = HierarchicJoint(Vector3f(-0.061f, 0.129f, -0.0175f), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	//rightShoulder = HierarchicJoint(Vector3f(-0.029f, 0.116f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	rightShoulder = HierarchicJoint(Vector3f(-0.029f, 0.174f, 0), eulerToQuaternion(Vector3f(6.925f, 0, 0)), scale);
	rightShoulder.setParent(&chest);
	addJoint(&rightShoulder, Joint::JointNames::SHOULDER_R);

	// LeftArm
	// 0.131675, -0.042729, -0.007322
	// 0.138628
	//leftArm = HierarchicJoint(Vector3f(0.139f, -0.0334f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftArm = HierarchicJoint(Vector3f(0.139f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftArm.setParent(&leftShoulder);
	addJoint(&leftArm, Joint::JointNames::ARM_L);

	// RightArm
	//-0.127931, -0.019725, 0.049677
	// 0.138648
	//rightArm = HierarchicJoint(Vector3f(-0.139f, -0.0334f, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightArm = HierarchicJoint(Vector3f(-0.139f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightArm.setParent(&rightShoulder);
	addJoint(&rightArm, Joint::JointNames::ARM_R);

	// LeftForeArm
	// 0.038594, -0.276205, -0.032969
	// 0.280830
	//leftForeArm = HierarchicJoint(Vector3f(0.256f, 0, 0), eulerToQuaternion(Vector3f(0, -0.1f, 0)), scale);
	leftForeArm = HierarchicJoint(Vector3f(0.281f, 0, 0), eulerToQuaternion(Vector3f(0, -1, 0)), scale);
	leftForeArm.setParent(&leftArm);
	addJoint(&leftForeArm, Joint::JointNames::FOREARM_L);

	// RightForeArm
	// 0.066347, -0.255619, 0.095603
	// 0.280861
	//rightForeArm = HierarchicJoint(Vector3f(-0.256f, 0, 0), eulerToQuaternion(Vector3f(0, 0.1f, 0)), scale);
	rightForeArm = HierarchicJoint(Vector3f(-0.281f, 0, 0), eulerToQuaternion(Vector3f(0, 1, 0)), scale);
	rightForeArm.setParent(&rightArm);
	addJoint(&rightForeArm, Joint::JointNames::FOREARM_R);

	// LeftHand
	// -0.002109, -0.244079, 0.032894
	// 0.246295
	//leftHand = HierarchicJoint(Vector3f(0.234f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftHand = HierarchicJoint(Vector3f(0.246f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	leftHand.setParent(&leftForeArm);
	addJoint(&leftHand, Joint::JointNames::HAND_L);

	// RightHand
	// 0.066648, -0.233974, 0.039119
	// 0.246406
	//rightHand = HierarchicJoint(Vector3f(-0.234f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightHand = HierarchicJoint(Vector3f(-0.246f, 0, 0), eulerToQuaternion(Vector3f(0, 0, 0)), scale);
	rightHand.setParent(&rightForeArm);
	addJoint(&rightHand, Joint::JointNames::HAND_R);

	invalidateGlobals();
}

void HierarchicSkeleton::init(Skeleton otherSkeleton) {

	init();

	for (auto& joint : joints) {
		joint->setLocalRotation(Quaternionf::Identity());
	}

	for (auto& joint : joints) {
		joint->invalidateGlobal();
	}

	hips.convertFromSkeleton(otherSkeleton.m_joints[Joint::HIPS]);

	leftUpLeg.convertFromSkeleton(otherSkeleton.m_joints[Joint::UPLEG_L], otherSkeleton.m_joints[Joint::HIPS]);
	rightUpLeg.convertFromSkeleton(otherSkeleton.m_joints[Joint::UPLEG_R], otherSkeleton.m_joints[Joint::HIPS]);

	leftLeg.convertFromSkeleton(otherSkeleton.m_joints[Joint::LEG_L], otherSkeleton.m_joints[Joint::UPLEG_L]);
	rightLeg.convertFromSkeleton(otherSkeleton.m_joints[Joint::LEG_R], otherSkeleton.m_joints[Joint::UPLEG_R]);

	leftFoot.convertFromSkeleton(otherSkeleton.m_joints[Joint::FOOT_L], otherSkeleton.m_joints[Joint::LEG_L]);
	rightFoot.convertFromSkeleton(otherSkeleton.m_joints[Joint::FOOT_R], otherSkeleton.m_joints[Joint::LEG_R]);

	leftToe.convertFromSkeleton(otherSkeleton.m_joints[Joint::TOE_L], otherSkeleton.m_joints[Joint::FOOT_L]);
	rightToe.convertFromSkeleton(otherSkeleton.m_joints[Joint::TOE_R], otherSkeleton.m_joints[Joint::FOOT_R]);

	spine.convertFromSkeleton(otherSkeleton.m_joints[Joint::SPINE], otherSkeleton.m_joints[Joint::HIPS]);
	chest.convertFromSkeleton(otherSkeleton.m_joints[Joint::CHEST], otherSkeleton.m_joints[Joint::SPINE]);
	neck.convertFromSkeleton(otherSkeleton.m_joints[Joint::NECK], otherSkeleton.m_joints[Joint::CHEST]);
	head.convertFromSkeleton(otherSkeleton.m_joints[Joint::HEAD], otherSkeleton.m_joints[Joint::NECK]);

	leftShoulder.convertFromSkeleton(otherSkeleton.m_joints[Joint::SHOULDER_L], otherSkeleton.m_joints[Joint::CHEST]);
	rightShoulder.convertFromSkeleton(otherSkeleton.m_joints[Joint::SHOULDER_R], otherSkeleton.m_joints[Joint::CHEST]);

	leftArm.convertFromSkeleton(otherSkeleton.m_joints[Joint::ARM_L], otherSkeleton.m_joints[Joint::SHOULDER_L]);
	rightArm.convertFromSkeleton(otherSkeleton.m_joints[Joint::ARM_R], otherSkeleton.m_joints[Joint::SHOULDER_R]);

	leftForeArm.convertFromSkeleton(otherSkeleton.m_joints[Joint::FOREARM_L], otherSkeleton.m_joints[Joint::ARM_L]);
	rightForeArm.convertFromSkeleton(otherSkeleton.m_joints[Joint::FOREARM_R], otherSkeleton.m_joints[Joint::ARM_R]);

	leftHand.convertFromSkeleton(otherSkeleton.m_joints[Joint::HAND_L], otherSkeleton.m_joints[Joint::FOREARM_L]);
	rightHand.convertFromSkeleton(otherSkeleton.m_joints[Joint::HAND_R], otherSkeleton.m_joints[Joint::FOREARM_R]);

	for (auto& joint : joints) {
		joint->setLocalRotation(Quaternionf::Identity());
	}

	leftLeg.setLocalRotation(eulerToQuaternion(Vector3f(0.1f, 0, 0)));
	rightLeg.setLocalRotation(eulerToQuaternion(Vector3f(0.1f, 0, 0)));

	leftFoot.setLocalRotation(eulerToQuaternion(Vector3f(-0.1f, 0, 0)));
	rightFoot.setLocalRotation(eulerToQuaternion(Vector3f(-0.1f, 0, 0)));

	spine.setLocalRotation(eulerToQuaternion(Vector3f(-6.925f, 0, 0)));
	neck.setLocalRotation(eulerToQuaternion(Vector3f(6.925f, 0, 0)));
	
	leftShoulder.setLocalRotation(eulerToQuaternion(Vector3f(6.925f, 0, 0)));
	rightShoulder.setLocalRotation(eulerToQuaternion(Vector3f(6.925f, 0, 0)));

	leftForeArm.setLocalRotation(eulerToQuaternion(Vector3f(0, -0.1f, 0)));
	rightForeArm.setLocalRotation(eulerToQuaternion(Vector3f(0, 0.1f, 0)));

	leftHand.setLocalRotation(eulerToQuaternion(Vector3f(0, 0.1f, 0)));
	rightHand.setLocalRotation(eulerToQuaternion(Vector3f(0, -0.1f, 0)));

	// TODO scale
	//scale = Vector3f::Ones();
}

void HierarchicSkeleton::reset() {

	//hips.setScale(Vector3f::Ones());
	scale = Vector3f::Ones();

	if (useXsens) {
		initXsens();
	}
	else {
		init();
	}

	invalidateGlobals();
}

void HierarchicSkeleton::invalidateGlobals() {

	for (auto& joint : joints) {
		joint->invalidateGlobal();
	}
}

void HierarchicSkeleton::insert() {

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
	}
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
