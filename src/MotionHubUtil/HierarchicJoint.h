#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/MMHmath.h"
#include "MotionHubUtil/Joint.h"

#include <vector>

class MotionHubUtil_DLL_import_export HierarchicJoint {

protected:

	HierarchicJoint* parent;

	std::vector<HierarchicJoint*> children;

	Joint::JointNames joint;

	Joint::JointConfidence confidence;

	Matrix4f local;

	Matrix4f global;

	bool globalValid;

public:

	HierarchicJoint(Joint::JointNames jointName, Vector3f localPosition, Quaternionf localRotation);

	HierarchicJoint(Vector3f localPosition, Quaternionf localRotation);

	HierarchicJoint(Vector3f localPosition);

	HierarchicJoint();

	void setParent(HierarchicJoint* hierarchicJoint);

	HierarchicJoint* getParent();

	void addChild(HierarchicJoint* child);

	std::vector<HierarchicJoint*> getChildren();

	HierarchicJoint* getFirstChild();

	void setJointName(Joint::JointNames jointName);

	Joint::JointNames getJointName();

	void setLocalPosition(Vector3f localPosition);

	void setLocalRotation(Quaternionf localRotation);

	Vector3f getLocalPosition();

	Vector3f getLocalPosition(Matrix4f matrix);

	Quaternionf getLocalRotation();

	Quaternionf getLocalRotation(Matrix4f matrix);

	void setGlobalPosition(Vector3f globalPosition);

	void setGlobalRotation(Quaternionf globalRotation);

	Vector3f getGlobalPosition();

	Quaternionf getGlobalRotation();

	Vector4f getLocalPosition4();

	Vector4f getGlobalPosition4();

	Vector3f getRight();

	Vector3f getUp();

	Vector3f getForward();

	Joint toJoint(Quaternionf rotation, Joint::JointConfidence confidence);

	Joint toJoint(Joint::JointConfidence confidence);

	Joint toJoint(Quaternionf rotation);

	Joint toJoint();

	void setConfidence(Joint::JointConfidence confidence);

	Vector3f inverseTransformDirection(Vector3f direction);

protected:

	Matrix4f combineParentMatrixRecursive(HierarchicJoint* hierarchicJoint);

	void invalidateGlobalRecursive(HierarchicJoint* hierarchicJoint);

};
