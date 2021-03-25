#pragma once

//include Eigen Library
#include <Dense>

#define M_PI 3.141592653589793238462643383279502884L

#define M_Deg2Rad 0.0174532924F
#define M_Rad2Deg 57.29578F

using namespace Eigen;

// TODO: Kommentieren

template <typename T> static int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

static float rsqrt(float length) {
	return 1.0f / sqrt(length);
}

static float lerp(const float a, const float b, const float t) {
	return a + t * (b - a);
}

static float clamp(const float x, const float upper, const float lower) {
	return fmin(upper, fmax(x, lower));
}

static Matrix3f getMatrix3x3f(Vector3f a, Vector3f b, Vector3f c) {
	Matrix3f matrix;

	matrix.col(0) = a;
	matrix.col(1) = b;
	matrix.col(2) = c;

	/*
	matrix(0, 0) = a.x();
	matrix(0, 1) = a.y();
	matrix(0, 2) = a.z();

	matrix(1, 0) = b.x();
	matrix(1, 1) = b.y();
	matrix(1, 2) = b.z();

	matrix(2, 0) = c.x();
	matrix(2, 1) = c.y();
	matrix(2, 2) = c.z();
	*/

	return matrix;
}

// https://github.com/Unity-Technologies/Unity.Mathematics/blob/4915b7afebc50b9c6c9a410b7a86ae5489aa6b9c/src/Unity.Mathematics/quaternion.cs#L405
/*!
 * Returns a quaternion view rotation given a forward vector and an up vector.
 * The two input vectors are not assumed to be unit length.
 * If the magnitude of either of the vectors is so extreme that the calculation cannot be carried out reliably or the vectors are collinear, the identity will be returned instead.
 *
 * \param forward The view forward direction
 * \param up The view up direction
 * \return The quaternion view rotation or the identity quaternion
 */
static Quaternionf lookRotation(Vector3f forward, Vector3f up) {
	/*
	if (forward == Vector3f::Zero()) {
		return Quaternionf::Identity();
	}

	if (forward != up) {
		Vector3f upNorm = up.normalized();

		Vector3f v = forward + upNorm * -upNorm.dot(forward);
		Quaternionf q = Quaternionf::FromTwoVectors(Vector3f(0, 0, 1), v);
		return Quaternionf::FromTwoVectors(v, forward) * q;
	}
	else {
		return Quaternionf::FromTwoVectors(Vector3f(0, 0, 1), forward);
	}
	*/

	float forwardLengthSq = forward.dot(forward);
	float upLengthSq = up.dot(up);

	forward *= rsqrt(forwardLengthSq);
	up *= rsqrt(upLengthSq);

	Vector3f t = up.cross(forward);
	float tLengthSq = t.dot(t);
	t *= rsqrt(tLengthSq);

	float mn = fmin(fmin(forwardLengthSq, upLengthSq), tLengthSq);
	float mx = fmax(fmax(forwardLengthSq, upLengthSq), tLengthSq);

	bool accept = mn > 1e-35f && mx < 1e35f && isfinite(forwardLengthSq) && isfinite(upLengthSq) && isfinite(tLengthSq);
	if (accept) {
		return Quaternionf(getMatrix3x3f(t, forward.cross(t), forward));
	}
	else {
		return Quaternionf::Identity();
	}
}

static Quaternionf rotationToLocalSpace(Quaternionf space, Quaternionf rotation) {
	return (space.inverse() * rotation).inverse();
}

static Quaternionf fromToRotation(Quaternionf from, Quaternionf to) {
	return to * from.inverse();
}

static Quaternionf fromToRotation(Vector3f from, Vector3f to) {
	return Quaternionf::FromTwoVectors(from, to);
}

static Quaternionf rotateTowards(Quaternionf from, Quaternionf to, float maxDegreesDelta) {
	float angle = from.angularDistance(to);
	if (angle == 0.0f) return to;

	return from.slerp(fmin(1.0f, maxDegreesDelta / angle), to);
}

static Quaternionf clampRotation(Quaternionf rotation, float clampWeight, int clampSmoothing) {
	Quaternionf identity = Quaternionf::Identity();

	if (clampWeight >= 1.0f) {
		return identity;
	}

	if (clampWeight <= 0.0f) {
		return rotation;
	}

	float angle = identity.angularDistance(rotation);
	float dot = 1.0f - (angle / 180.0f);

	float targetClampMlp = clamp(1.0f - ((clampWeight - dot) / (1.0f - dot)), 0.0f, 1.0f);
	float clampMlp = clamp(dot / clampWeight, 0.0f, 1.0f);

	for (int i = 0; i < clampSmoothing; i++) {
		float sinF = clampMlp * M_PI * 0.5f;
		clampMlp = sinf(sinF);
	}

	return identity.slerp(clampMlp * targetClampMlp, rotation);
}

// https://github.com/Unity-Technologies/UnityCsReference/blob/61f92bd79ae862c4465d35270f9d1d57befd1761/Runtime/Export/Math/Vector3.cs#L275
static Vector3f project(Vector3f vector, Vector3f onNormal) {

	float sqrMag = onNormal.dot(onNormal);

	if (sqrMag < FLT_EPSILON) {
		return Vector3f::Zero();
	}
	else {
		float dot = vector.dot(onNormal);

		return Vector3f(
			onNormal.x() * dot / sqrMag,
			onNormal.y() * dot / sqrMag,
			onNormal.z() * dot / sqrMag
		);
	}
}

// https://graemepottsfolio.wordpress.com/2015/11/26/vectors-programming/
static void orthoNormalize(Vector3f& normal, Vector3f& tangent) {
	/*normal.normalize();
	tangent.normalize();

	tangent.completeOrthogonalDecomposition();

	return tangent.cross(normal);*/

	tangent = (tangent - project(tangent, normal.normalized())).normalized();
}

// https://graemepottsfolio.wordpress.com/2015/11/26/vectors-programming/
static void orthoNormalize(Vector3f& normal, Vector3f& tangent, Vector3f& binormal) {

	orthoNormalize(normal, tangent);

	binormal = binormal - project(binormal, normal);
	binormal = (binormal - project(binormal, tangent)).normalized();
}

// https://github.com/Unity-Technologies/UnityCsReference/blob/61f92bd79ae862c4465d35270f9d1d57befd1761/Runtime/Export/Math/Vector3.cs#L305

/*!
 * Returns the angle in degrees between vector from and vector to. This is always the smallest
 *
 * \param from The first vector.
 * \param to The second vector.
 * \return The angle in degrees between both vectors.
 */
static float angle(Vector3f from, Vector3f to) {
	// sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
	float denominator = sqrt(from.squaredNorm() * to.squaredNorm());

	float kEpsilonNormalSqrt = 1e-15F;

	if (denominator < kEpsilonNormalSqrt) {
		return 0.0F;
	}
	float dot = clamp(from.dot(to) / denominator, -1.0f, 1.0f);

	return acosf(dot) * M_Rad2Deg;
}

/*!
 * The smaller of the two possible angles between the two vectors is returned, therefore the result will never be greater than 180 degrees or smaller than -180 degrees.
 * If you imagine the from and to vectors as lines on a piece of paper, both originating from the same point, then the /axis/ vector would point up out of the paper.
 * 
 * \param from The first vector.
 * \param to The second vector.
 * \param axis The axis vector.
 * \return The measured angle between the two vectors. positive in a clockwise direction and negative in an anti-clockwise direction.
 */
static float signedAngle(const Vector3f& from, const Vector3f& to, const Vector3f& axis) {
	float unsignedAngle = angle(from, to);

	float cross_x = from.y() * to.z() - from.z() * to.y();
	float cross_y = from.z() * to.x() - from.x() * to.z();
	float cross_z = from.x() * to.y() - from.y() * to.x();

	float sign = sgn(axis.x() * cross_x + axis.y() * cross_y + axis.z() * cross_z);

	return unsignedAngle * sign;
}

// https://github.com/Unity-Technologies/Unity.Mathematics/blob/4915b7afebc50b9c6c9a410b7a86ae5489aa6b9c/src/Unity.Mathematics/quaternion.cs#L99
/*!
 * Returns a quaternion representing a rotation around a unit axis by an angle in radians.
 * The rotation direction is clockwise when looking along the rotation axis towards the origin.
 *
 * \param angle The angle of rotation in radians.
 * \param axis The axis of rotation.
 * \return The quaternion representing a rotation around an axis.
 */
static Quaternionf axisAngle(const float& angle, const Vector3f& axis) {
	float halfAngle = 0.5f * angle;

	float sina = sinf(halfAngle);
	float cosa = cosf(halfAngle);

	Vector3f axisAngle = axis * sina;

	return Quaternionf(cosa, axisAngle.x(), axisAngle.y(), axisAngle.z());
}

/*!
 * Returns the distance between vector a and  vector b.
 *
 * \param a The first vector.
 * \param b The second vector.
 * \return The distance between both vectors.
 */
static float distance(const Vector3f& a, const Vector3f& b) {
	float diff_x = a.x() - b.x();
	float diff_y = a.y() - b.y();
	float diff_z = a.z() - b.z();

	return sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
}