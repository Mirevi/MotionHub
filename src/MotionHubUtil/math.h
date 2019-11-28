#pragma once

#include <Core>
#include <Geometry>
#include <Dense>

#include "Vector3.h"


# define M_PI 3.141592653589793238462643383279502884L /* pi */

using namespace Eigen;



/*!
 * converts transform vectors into transform matrix
 * 
 * \param position position vector
 * \param rotation rotation vector
 * \param scale scale vector
 * \return transform matrix
 */
static Eigen::Matrix4f* transformMatrix(Vector3 position, Vector3 rotation, Vector3 scale)
{

	//create matrix pointer
	Eigen::Matrix4f* mTransform = new Eigen::Matrix4f();

	//create rotation quaternion with rotation values
	Quaternion<float> qRotation;
	qRotation = AngleAxisf(rotation.m_xyz.x / 180 * M_PI, Vector3f::UnitX()) *
				AngleAxisf(rotation.m_xyz.y / 180 * M_PI, Vector3f::UnitY()) *
				AngleAxisf(rotation.m_xyz.z / 180 * M_PI, Vector3f::UnitZ());


	//convert quaternion to 3x3 matrix and scale with scaling values
	Matrix3f mRotation = qRotation.matrix() * Scaling(scale.m_xyz.x, scale.m_xyz.y, scale.m_xyz.z);

	//create translation matrix with translation vector
	Vector3f mTranslation(position.m_xyz.x, position.m_xyz.y, position.m_xyz.z);
	

	// Set to Identity to make bottom row of Matrix 0,0,0,1
	mTransform->setIdentity();

	//insert rot/scale matrix and translation vector into final matrix
	mTransform->block<3, 3>(0, 0) = mRotation;
	mTransform->block<3, 1>(0, 3) = mTranslation;


	return mTransform;
}