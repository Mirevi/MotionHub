#include "IKSolverSpine.h"

IKSolverSpine::IKSolverSpine() {

	hip = nullptr;
}

IKSolverSpine::IKSolverSpine(HierarchicJoint* spine, HierarchicJoint* chest, HierarchicJoint* neck, HierarchicJoint* head) {

	// Create IKJoint objects from given joints
	spineJoint = IKJoint(spine);
	chestJoint = IKJoint(chest);
	neckJoint = IKJoint(neck);
	headJoint = IKJoint(head);
}

void IKSolverSpine::setHip(HierarchicJoint* joint) {

	// Store reference to hip joint
	hip = joint;
}

void IKSolverSpine::init() {

	// Init CCD joints (Loop is backwards!)
	ccdJoints.push_back(&chestJoint);
	ccdBendWeights.push_back(0.8f); // 1.0?

	ccdJoints.push_back(&spineJoint);
	ccdBendWeights.push_back(0.65f); // 0.33 - 0.66 (0.5?)

	// Init Twist joints
	twistJoints.push_back(&spineJoint);
	twistWeights.push_back(0.5f);

	twistJoints.push_back(&chestJoint);
	twistWeights.push_back(0.5f);

	twistJoints.push_back(&neckJoint);
	twistWeights.push_back(1.0f);

	// Save default state for joints
	saveDefaultState();
}

void IKSolverSpine::solve(Vector3f position, Quaternionf rotation) {

	// Call solve from base class
	IKSolver::solve(position, rotation);

	// Restore default state
	loadDefaultState();

	// Solve joint positions towards head position with a CCD iteration
	solveCCD();

	// Twist joint rotations towards head rotation
	solveTwist();

	// Solve another CCD iteration to make the spine less rigid
	solveCCD();

	// Solve neck joint with CCD
	Quaternionf fromTo = IKSolver::solveCCD(neckJoint.getPosition(), headJoint.getPosition(), targetPosition);
	neckJoint.setRotation(fromTo * neckJoint.getRotation());

	// Solve Head rotation
	Quaternionf headRotation = fromToRotation(headJoint.getRotation(), targetRotation);
	headJoint.setRotation(headRotation * headJoint.getRotation());
	//headJoint.setRotation(targetRotation);
}

void IKSolverSpine::saveDefaultState() {

	// Clear default rotations
	defaultRotations.clear();

	// Save default state for all joints
	defaultRotations.push_back(spineJoint.getLocalRotation());
	defaultRotations.push_back(chestJoint.getLocalRotation());
	defaultRotations.push_back(neckJoint.getLocalRotation());
	defaultRotations.push_back(headJoint.getLocalRotation());

	// Calculate relative rotation between head & hip
	headRelativeToHip = hip->getGlobalRotation().inverse() * headJoint.getRotation();
}

void IKSolverSpine::loadDefaultState() {

	// Load default state for all joints
	spineJoint.setLocalRotation(defaultRotations[0]);
	chestJoint.setLocalRotation(defaultRotations[1]);
	neckJoint.setLocalRotation(defaultRotations[2]);
	headJoint.setLocalRotation(defaultRotations[3]);
}

void IKSolverSpine::solveCCD() {

	// Store head position
	Vector3f headPosition = headJoint.getPosition();

	// Loop over all CCD joints
	for (int i = 0; i < ccdJoints.size(); i++) {

		// Store current joint position & rotation
		Vector3f jointPosition = ccdJoints[i]->getPosition();
		Quaternionf jointRotation = ccdJoints[i]->getRotation();

		// Create rotation towards target with CCD
		Quaternionf fromTo = IKSolver::solveCCD(jointPosition, headPosition, targetPosition);

		// Apply rotation to joint rotation
		Quaternionf rotation = fromTo * jointRotation;
		
		// Limit rotation to XZ only?
		if (onlyXZCCD || lerpTest) {

			// Decompose x rotation from right vector
			Quaternionf xRotation = decomposeTwist(fromTo, jointRotation * Vector3f(1, 0, 0));

			// Decompose z rotation from forward vector
			Quaternionf zRotation = decomposeTwist(fromTo, jointRotation * Vector3f(0, 0, 1));

			// Create new rotation from XZ & joint rotation
			rotation = (xRotation * zRotation) * jointRotation;

			if (lerpTest) {
				rotation = fromTo.slerp(0.5f, xRotation * zRotation) * jointRotation;
			}
		}
		
		// Calculate angle between current and new rotation
		//float angle = angleBetween(jointRotation, rotation);
		
		// Calculate delta for current joint
		// TODO: 0.5f / 0.3333?
		float delta = ccdWeight * 0.5f * ccdBendWeights[i];

		// Set joint rotation towards target with max degrees delta
		//Quaternionf rotationTowards = rotateTowards(jointRotation, rotation, angle);
		Quaternionf rotationTowards = jointRotation.slerp(delta, rotation);
		ccdJoints[i]->setRotation(rotationTowards);

	}
}

void IKSolverSpine::solveTwist() {

	// Calculate rotation relative to hip
	Quaternionf rotation = targetRotation * (hip->getGlobalRotation() * headRelativeToHip).inverse();

	// Calculate proportion for all joints
	float proportion = 1.0f / twistJoints.size();

	// Loop over all Twist joints
	for (int i = 0; i < twistJoints.size(); i++) {

		// Store current joint rotation
		Quaternionf jointRotation = twistJoints[i]->getRotation();

		// Decompose y rotation from up vector
		Quaternionf yRotation = decomposeTwist(rotation, jointRotation * Vector3f(0, 1, 0));

		// Lerp between rotation & y only rotation
		Quaternionf newRotation = rotation.slerp(twistLerpY, yRotation);

		// Calculate delta for current joint
		float delta = twistWeight * proportion * twistWeights[i];

		// Twist joint towards head rotation with current delta
		Quaternionf identity = Quaternionf::Identity();
		Quaternionf rotationTowards = identity.slerp(delta, newRotation) * jointRotation;

		// Set joint rotation
		twistJoints[i]->setRotation(rotationTowards);
	}
}