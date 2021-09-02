#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/HierarchicJoint.h"

/*!
 * \class IKJoint
 *
 * \brief Wraps HierarchicJoint and offers functionality for IK Solvers
 */
class MotionHubUtil_DLL_import_export IKJoint {

public:

	/*!
	 * Default constructor. Sets joint to null
	 */
	IKJoint() {

		joint = nullptr;
	}

	/*!
	 * Constructor with given joint
	 */
	IKJoint(HierarchicJoint* hierarchicJoint) {

		joint = hierarchicJoint;
	}

	/*!
	 * Initializes Joint with a direction to child joint & normal on
	 *
	 * \param directionToChild direction from joint position to child position
	 * \param normal the normal bend
	 */
	void init(Vector3f jointToChild, Vector3f normal) {

		// Get joint rotation and calculate vector to child joint
		Quaternionf jointRotation = joint->getGlobalRotation();

		// Store default normal in joint space
		defaultLocalNormal = jointRotation.inverse() * normal;

		// Store Quaternion for tansforming rotations to joint space
		Quaternionf jointToChildRotation = lookRotation(jointToChild, normal);
		toJointSpace = rotationToSpace(jointRotation, jointToChildRotation);

		// Store square joint length & sqrMagnitude for calculations/checks
		squaredNorm = jointToChild.squaredNorm();
		length = sqrtf(squaredNorm);
	}

	/*!
	 * Saves joints default local position & rotation
	 */
	void saveDefaultState() {

		defaultPosition = joint->getLocalPosition();
		defaultRotation = joint->getLocalRotation();
	}

	/*!
	 * Restores joints default local position & rotation
	 */
	void loadDefaultState() {

		joint->setLocalPosition(defaultPosition);
		joint->setLocalRotation(defaultRotation);
	}
	
	/*!
	 * Sets joints solved position with given position
	 * 
	 * \param position the solved position
	 */
	void setSolvedPosition(Vector3f position) {

		solvedPosition = position;
	}

	/*!
	 * Saves joints global position as solved position
	 */
	void saveSolvedPosition() {

		setSolvedPosition(getPosition());
	}

	void writeSolvedPosition() {
		setPosition(solvedPosition);
	}

	/*!
	 * Returns the joints solved position
	 * 
	 * \return the solved position
	 */
	Vector3f getSolvedPosition() {

		return solvedPosition;
	}

	/*!
	 * Sets joints solved position with given position
	 *
	 * \param position the solved position
	 */
	void setSolvedRotation(Quaternionf rotation) {

		solvedRotation = rotation;
	}

	/*!
	 * Saves joints global position as solved position
	 */
	void saveSolvedRotation() {

		setSolvedRotation(getRotation());
	}

	void writeSolvedRotation() {
		setRotation(solvedRotation);
	}

	/*!
	 * Returns the joints solved position
	 *
	 * \return the solved position
	 */
	Quaternionf getSolvedRotation() {

		return solvedRotation;
	}

	/*!
	 * Sets the rotation to a direction in joint space with default normal
	 *
	 * \param direction the look direction
	 */
	void setRotationTowards(Vector3f direction) {

		Vector3f normal = getRotation() * defaultLocalNormal;

		setRotationTowards(direction, normal);
	}

	/*!
	 * Sets the rotation to a direction with a normal in joint space
	 *
	 * \param direction the look direction
	 * \param normal the up normal
	 */
	void setRotationTowards(Vector3f direction, Vector3f normal) {
		Quaternionf rotation = lookRotation(direction, normal) * toJointSpace;

		setRotation(rotation);
	}

	void setPosition(Vector3f position) {

		joint->setGlobalPosition(position);
	}

	Vector3f getPosition() {

		return joint->getGlobalPosition();
	}

	void setLocalPosition(Vector3f position) {

		joint->setLocalPosition(position);
	}

	Vector3f getLocalPosition() {

		return joint->getLocalPosition();
	}

	void setRotation(Quaternionf rotation) {

		joint->setGlobalRotation(rotation);
	}

	Quaternionf getRotation() {

		return joint->getGlobalRotation();
	}

	void setLocalRotation(Quaternionf rotation) {

		joint->setLocalRotation(rotation);
	}

	Quaternionf getLocalRotation() {

		return joint->getLocalRotation();
	}

public:

	// TODO: IKJoint von HierarchicJoint erben lassen?
	HierarchicJoint* joint;

	// TODO: Zusätzlich length speichern? -> sqrt vermeiden
	float squaredNorm = 0;
	float length = 0;

protected:

	Vector3f solvedPosition;
	Quaternionf solvedRotation;

	Vector3f defaultPosition;
	Quaternionf defaultRotation;

	Quaternionf toJointSpace;
	Vector3f defaultLocalNormal;

	Quaternionf childRotation;
	Quaternionf savedChildRotation;

	float lastConfidentAngle = 0;
};