#pragma once

#include "ConfigDllExportMotionHubUtil.h"
#include <iostream>

#include <Dense>
//#include <src/plugins/BlockMethods.h>

#define M_PI 3.141592653589793238462643383279502884L // pi
#define Quaternion Quaternionf

using namespace Eigen;



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
	mRotation = AngleAxisf(rotation.x() / 180.0f * M_PI, Vector3f::UnitX())
			  * AngleAxisf(rotation.y() / 180.0f * M_PI, Vector3f::UnitY())
			  * AngleAxisf(rotation.z() / 180.0f * M_PI, Vector3f::UnitZ());
	
	//scale rotation matrix with scaling values
	mRotation *= Scaling(scale.x(), scale.y(), scale.z());

	//Set to Identity to make bottom row of Matrix 0,0,0,1
	mTransform.setIdentity();

	//insert rot/scale matrix and translation vector into final matrix
	mTransform.block<3, 3>(0, 0) = mRotation;
	mTransform.block<3, 1>(0, 3) = position;


	return mTransform;
}

static std::string toString(Eigen::Vector4f vector)
{

	std::string string;

	string = std::to_string(vector.x()) + ", " + std::to_string(vector.y()) + ", " + std::to_string(vector.z()) + ", " + std::to_string(vector.w());

	return string;

}

static std::string toString(Eigen::Quaternionf quaternion)
{

	std::string string;

	string = std::to_string(quaternion.x()) + ", " + std::to_string(quaternion.y()) + ", " + std::to_string(quaternion.z()) + ", " + std::to_string(quaternion.w());

	return string;

}