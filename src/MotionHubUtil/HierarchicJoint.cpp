#include "HierarchicJoint.h"

HierarchicJoint::HierarchicJoint(Joint::JointNames jointName, Vector3f localPosition, Quaternionf localRotation)
	: HierarchicJoint(localPosition, localRotation)
{
	setJointName(jointName);

	globalValid = false;
}

HierarchicJoint::HierarchicJoint(Vector3f localPosition, Quaternionf localRotation) {
	parent = nullptr;

	local.setIdentity();

	confidence = Joint::JointConfidence::MEDIUM;

	setLocalPosition(localPosition);
	setLocalRotation(localRotation);

	//global = local;
	scale = Vector3f(1.0f, 1.0f, 1.0f);
	globalValid = false;
}

HierarchicJoint::HierarchicJoint(Vector3f localPosition)
	: HierarchicJoint(localPosition, Quaternionf::Identity())
{ }

HierarchicJoint::HierarchicJoint()
	: HierarchicJoint(Vector3f::Zero(), Quaternionf::Identity())
{ }

void HierarchicJoint::setParent(HierarchicJoint* hierarchicJoint) {
	if (hierarchicJoint == nullptr) {
		return;
	}

	parent = hierarchicJoint;

	parent->addChild(this);

	invalidateGlobalRecursive(this);
}

HierarchicJoint* HierarchicJoint::getParent() {
	return parent;
}

void HierarchicJoint::addChild(HierarchicJoint* child) {
	children.push_back(child);
}

std::vector<HierarchicJoint*> HierarchicJoint::getChildren() {
	return children;
}

HierarchicJoint* HierarchicJoint::getFirstChild() {
	if (children.size() >= 1) {
		children[0];
	}

	return nullptr;
}

void HierarchicJoint::setJointName(Joint::JointNames jointName) {
	joint = jointName;
}

Joint::JointNames HierarchicJoint::getJointName() {
	return joint;
}

void HierarchicJoint::setLocalPosition(Vector3f localPosition) {
	local.block<3, 1>(0, 3) = localPosition;

	invalidateGlobalRecursive(this);
}

void HierarchicJoint::setLocalRotation(Quaternionf localRotation) {
	local.block<3, 3>(0, 0) = localRotation.toRotationMatrix();

	invalidateGlobalRecursive(this);
}

Vector3f HierarchicJoint::getLocalPosition() {
	return getLocalPosition(local);
}

Vector3f HierarchicJoint::getLocalPosition(Matrix4f matrix) {
	return matrix.block<3, 1>(0, 3);
}

Quaternionf HierarchicJoint::getLocalRotation() {
	return getLocalRotation(local);
}

Quaternionf HierarchicJoint::getLocalRotation(Eigen::Matrix4f matrix) {
	return Quaternionf(matrix.block<3, 3>(0, 0));
}

void HierarchicJoint::setGlobalPosition(Vector3f globalPosition) {
	Vector3f parentPosition = Vector3f::Zero();

	if (parent != nullptr) {
		parentPosition = parent->getGlobalPosition();
	}

	Vector3f localPosition = globalPosition - parentPosition;

	setLocalPosition(localPosition);
}

void HierarchicJoint::setGlobalRotation(Quaternionf globalRotation) {
	Quaternionf parentRotation = Quaternionf::Identity();

	if (parent != nullptr) {
		parentRotation = parent->getGlobalRotation();
	}

	Quaternionf localRotation = parentRotation.inverse() * globalRotation;

	setLocalRotation(localRotation);
}

Vector3f HierarchicJoint::getGlobalPosition() {
	if (parent != nullptr) {
		if (!globalValid) {
			global = combineParentMatrixRecursive(this);
			//globalValid = true;
		}

		return getLocalPosition(global);
	}
	else {
		return getLocalPosition();
	}
}

Quaternionf HierarchicJoint::getGlobalRotation() {
	if (parent != nullptr) {
		if (!globalValid) {
			global = combineParentMatrixRecursive(this);
			//globalValid = true;
		}

		return getLocalRotation(global);
	}
	else {
		return getLocalRotation();
	}
}

Vector4f HierarchicJoint::getLocalPosition4() {
	Vector3f pos = getLocalPosition();
	return Vector4f(pos.x(), pos.y(), pos.z(), 1.0f);
}

Vector4f HierarchicJoint::getGlobalPosition4() {
	Vector3f pos = getGlobalPosition();
	return Vector4f(pos.x(), pos.y(), pos.z(), 1.0f);
}

void HierarchicJoint::setScale(Vector3f scale) {
	this->scale = scale;
}

Vector3f HierarchicJoint::getScale() {
	return scale;
}

Vector3f HierarchicJoint::getRight() {
	return getGlobalRotation() * Vector3f(1, 0, 0);
}

Vector3f HierarchicJoint::getUp() {
	return getGlobalRotation() * Vector3f(0, 1, 0);
}

Vector3f HierarchicJoint::getForward() {
	return getGlobalRotation() * Vector3f(0, 0, 1);
}

Joint HierarchicJoint::toJoint(Quaternionf rotation, Joint::JointConfidence confidence) {
	return Joint(getGlobalPosition4(), rotation, confidence);
}

Joint HierarchicJoint::toJoint(Joint::JointConfidence confidence) {
	return toJoint(getGlobalRotation(), confidence);
}

Joint HierarchicJoint::toJoint(Quaternionf rotation) {
	return toJoint(rotation, confidence);
}

Joint HierarchicJoint::toJoint() {
	return toJoint(confidence);
}

void HierarchicJoint::setConfidence(Joint::JointConfidence confidence) {
	this->confidence = confidence;
}

Vector3f HierarchicJoint::inverseTransformDirection(Vector3f direction) {
	if (!globalValid) {
		global = combineParentMatrixRecursive(this);
	}

	return getLocalPosition(local.inverse()).array() * direction.array();
	//return getLocalPosition(local.inverse()) * direction;
}

Matrix4f HierarchicJoint::combineParentMatrixRecursive(HierarchicJoint* hierarchicJoint) {
	// Is parent not null? -> Joint
	if (hierarchicJoint->parent != nullptr) {
		if (parent->globalValid) {
			return parent->global * hierarchicJoint->local;
		}
		else {
			Matrix4f newGlobal = combineParentMatrixRecursive(hierarchicJoint->parent) * hierarchicJoint->local;

			hierarchicJoint->global = newGlobal;
			hierarchicJoint->globalValid = true;

			return newGlobal;
		}

	}
	// Parent is null -> RootJoint
	else {

		Vector3f scale = hierarchicJoint->scale;
		
		// Dont apply scaling if scale is (1, 1, 1)
		if (scale.isApprox(Vector3f::Ones())) {
			return hierarchicJoint->local;
		}
		else {
			// Copy matrix and apply scaling
			Matrix4f scaledLocal = Matrix4f(hierarchicJoint->local);

			scaledLocal(0, 0) *= scale.x();
			scaledLocal(1, 0) *= scale.y();
			scaledLocal(2, 0) *= scale.z();

			scaledLocal(0, 1) *= scale.x();
			scaledLocal(1, 1) *= scale.y();
			scaledLocal(2, 1) *= scale.z();

			scaledLocal(0, 2) *= scale.x();
			scaledLocal(1, 2) *= scale.y();
			scaledLocal(2, 2) *= scale.z();

			return scaledLocal;
		}
		//return hierarchicJoint->local;
	}
}

void HierarchicJoint::invalidateGlobalRecursive(HierarchicJoint* hierarchicJoint) {
	hierarchicJoint->globalValid = false;

	for (HierarchicJoint* child : hierarchicJoint->children) {
		invalidateGlobalRecursive(child);
	}
}
