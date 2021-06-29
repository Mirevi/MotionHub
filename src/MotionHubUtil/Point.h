#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include "MotionHubUtil/MMHmath.h"
#include "MotionHubUtil/Joint.h"

/*!
 * \class Point
 *
 * \brief Data Structure for Points with position and rotation
 *
 * \author David Nowottnik
 */
class MotionHubUtil_DLL_import_export Point {

public:

	enum PointType {
		Undefined,
		HMD,
		Controller,
		Tracker,
		Camera,
		Rigidbody,
		Marker
	};

	/*!
	 * default constructor
	 *
	 */
	Point();

	/*!
	 * constructor with undefinded type
	 *
	 * \param id
	 * \param position
	 * \param rotation
	 */
	Point(Vector3f position, Quaternionf rotation);

	/*!
	 * constructor with type
	 *
	 * \param id
	 * \param position
	 * \param rotation
	 */
	Point(Vector3f position, Quaternionf rotation, PointType type);

	/*!
	 * getter for Points Position Vector
	 */
	Vector3f getPosition();

	/*!
	 * setter for Points Position Vector
	 */
	void setPosition(Vector3f position);

	/*!
	 * getter for Points Rotation Quaternion
	 */
	Quaternionf getRotation();

	/*!
	 * setter for Points Rotation Quaternion
	 */
	void setRotation(Quaternionf rotation);

	/*!
	 * getter for PointType
	 */
	PointType getType();

	/*!
	 * setter for PointType
	 */
	void setType(Point::PointType type);

	/*!
	 * setter for JointName
	 */
	void setJointName(Joint::JointNames jointName);

	/*!
	 * getter for custom int
	 */
	int getCustomInt();

	/*!
	 * setter for custom int
	 */
	void setCustomInt(int custom);

	/*!
	 * getter for custom float
	 */
	float getCustomFloat();

	/*!
	 * setter for custom float
	 */
	void setCustomFloat(float custom);

private:

	Vector3f position;

	Quaternionf rotation;

	PointType type;

	int customInt = INT_MIN;

	float customFloat = FLT_MIN;
};