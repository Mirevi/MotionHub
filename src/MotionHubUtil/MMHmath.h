#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <iostream>

//include Eigen Library
#include <Dense>

#define M_PI 3.141592653589793238462643383279502884L // pi
#define Quaternion Quaternionf

using namespace Eigen;


/*!
 * converts Euler Angles to transform matrix (translation or scale)
 * 
 * \param euler input euler angles as Vector
 * \return tranform matrix
 */
static Matrix3f eulerToMatrix(Vector3f euler);

/*!
 * converts euler angles to rotationmatrix
 * 
 * \param euler input euler angles as Vector
 * \return rotation matrix
 */
static Quaternionf eulerToQuaternion(Vector3f euler);

/*!
 * converts transform vectors into transform matrix
 * 
 * \param position position vector
 * \param rotation rotation vector
 * \param scale scale vector
 * \return transform matrix
 */
static Matrix4f transformMatrix(Vector3f position, Vector3f rotation, Vector3f scale)
{

	//create matrix pointer
	Matrix4f mTransform;


	//create 3x3 matrix from euler angles
	Matrix3f mRotation;
	mRotation = eulerToMatrix(rotation);
	
	//scale rotation matrix with scaling values
	mRotation *= Scaling(scale.x(), scale.y(), scale.z());

	//Set to Identity to make bottom row of Matrix 0,0,0,1
	mTransform.setIdentity();

	//insert rot/scale matrix and translation vector into final matrix
	mTransform.block<3, 3>(0, 0) = mRotation;
	mTransform.block<3, 1>(0, 3) = position;


	return mTransform;
}

/*!
 * converts quaternion to euler angles
 * 
 * \param quaternion input quaternion
 * \return output euler angles as Vector
 */
static Vector3f quaternionToEuler(Quaternionf quaternion)
{
	//convert to euler angles as radient
	Vector3f euler = quaternion.toRotationMatrix().eulerAngles(1, 2, 0);

	//convert radient to degrees
	euler.x() *= 180;
	euler.x() /= M_PI;
	euler.y() *= 180;
	euler.y() /= M_PI;
	euler.z() *= 180;
	euler.z() /= M_PI;

	return euler;

}

/*!
 * converts euler angles to matrix
 * 
 * \param euler input euler angles as vector
 * \return output rotation matrix
 */
static Matrix3f eulerToMatrix(Vector3f euler)
{

	Matrix3f mRotation;

	mRotation = AngleAxisf(euler.x() / 180.0f * M_PI, Vector3f::UnitX())
			  * AngleAxisf(euler.y() / 180.0f * M_PI, Vector3f::UnitY())
			  * AngleAxisf(euler.z() / 180.0f * M_PI, Vector3f::UnitZ());
			   
	return mRotation;

}

/*!
 * convert euler angles to quaternion
 * 
 * \param euler input euler angles as Vector
 * \return output quaternion
 */
static Quaternionf eulerToQuaternion(Vector3f euler)
{

	Quaternionf qRotation;

	qRotation = AngleAxisf(euler.x() * M_PI / 180, Vector3f::UnitX())
			  * AngleAxisf(euler.y() * M_PI / 180, Vector3f::UnitY())
			  * AngleAxisf(euler.z() * M_PI / 180, Vector3f::UnitZ());
	
	qRotation = Quaternionf(qRotation.w(), qRotation.x(), qRotation.y(), qRotation.z());

	return qRotation;

}

/*!
 * converts Vector4 to string
 * 
 * \param vector input vector
 * \return output string
 */
static std::string toString(Eigen::Vector4f vector)
{

	std::string string;

	string = std::to_string(vector.x()) + ", " + std::to_string(vector.y()) + ", " + std::to_string(vector.z()) + ", " + std::to_string(vector.w());

	return string;

}


/*!
 * converts Vector3 to string
 * 
 * \param vectorinput vector
 * \return output string
 */
static std::string toString(Eigen::Vector3f vector)
{

	std::string string;

	string = std::to_string(vector.x()) + ", " + std::to_string(vector.y()) + ", " + std::to_string(vector.z());

	return string;

}

/*!
 * converts quaternion to string
 * 
 * \param quaternion input quaternion
 * \return output string
 */
static std::string toString(Quaternionf quaternion)
{

	std::string string;

	string = std::to_string(quaternion.x()) + ", " + std::to_string(quaternion.y()) + ", " + std::to_string(quaternion.z()) + ", " + std::to_string(quaternion.w());

	return string;

}