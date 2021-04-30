#pragma once

#include <Dense>

#define M_PI 3.141592653589793238462643383279502884L

#define M_Deg2Rad 0.01745329251994329576923690768489F
#define M_Rad2Deg 57.295779513082320876798154814105F

using namespace Eigen;

/*!
 * Returns the signum function of x
 * 
 * \return -1 if x<0, 0 if x=0, 1 if x>0
 */
template <typename T> static int sgn(T x) {
	return (T(0) < x) - (x < T(0));
}

/*!
 * Calculates the reverse square root of x
 * 
 * \return reverse square root of x
 */
static float rsqrt(float x) {
	return 1.0f / sqrt(x);
}

/*!
 * Returns the result of linearly interpolating from a to b using the interpolation parameter t
 * 
 * \remarks If the interpolation parameter is not in the range [0, 1], then this function extrapolates
 * \param a zhe first endpoint, corresponding to the interpolation parameter value of 0
 * \param b The second endpoint, corresponding to the interpolation parameter value of 1
 * \param t The interpolation parameter. May be a value outside the interval [0, 1]
 * \return The interpolation from x to y
 */
static float lerp(const float a, const float b, const float t) {
	return a + t * (b - a);
}

/*!
 * Clamps x between min and max and returns value
 * 
 * \param x input value to be clamped
 * \param min lower bound of the interval
 * \param max upper bound of the interval
 * \return x between min and max
 */
static float clamp(const float x, const float min, const float max) {
	return fmax(min, fmin(max, x));
}

/*!
 * Clamps x between 0 and 1 and returns value
 * 
 * \param x input value to be clamped
 * \return x between 0 and 1
 */
static float clamp01(const float x) {
	return clamp(x, 0.0f, 1.0f);
}

/*!
 * Returns a quaternion view rotation given a forward vector and an up vector
 * The two input vectors are not assumed to be unit length
 * If the magnitude of either of the vectors is so extreme that the calculation cannot be carried out reliably or the vectors are collinear, the identity will be returned instead
 *
 * \param forward The view forward direction
 * \param up The view up direction
 * \return The quaternion view rotation or the identity quaternion
 * \see https://github.com/Unity-Technologies/Unity.Mathematics/blob/4915b7afebc50b9c6c9a410b7a86ae5489aa6b9c/src/Unity.Mathematics/quaternion.cs#L405
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
	
	// TODO: Oben/Unten?
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

		Matrix3f matrix;
		matrix.col(0) = t;
		matrix.col(1) = forward.cross(t);
		matrix.col(2) = forward;

		return Quaternionf(matrix);
	}
	else {
		return Quaternionf::Identity();
	}
}

/*!
 * Transforms a rotation to a another rotational space
 *
 * \param space the rotational space to transform to
 * \param rotation lower bound of the interval
 * \return the transformed rotation 
 */
static Quaternionf rotationToSpace(Quaternionf space, Quaternionf rotation) {
	return (space.inverse() * rotation).inverse();
}

/*!
 * Creates a rotation which rotates from first direction to second direction
 *
 * \param from first direction
 * \param to second direction
 * \return the transformed direction
 */
static Quaternionf fromToRotation(Quaternionf from, Quaternionf to) {
	return to * from.inverse();
}

/*!
 * Creates a rotation which rotates from first direction to second direction
 *
 * \param from first direction
 * \param to second direction
 * \return the transformed direction
 */
static Quaternionf fromToRotation(Vector3f from, Vector3f to) {
	return Quaternionf::FromTwoVectors(from, to);
}

/*!
 * Rotates a rotation from towards to
 * The from quaternion is rotated towards to by an angular step of maxDegreesDelta (but note that the rotation will not overshoot)
 * Negative values of maxDegreesDelta will move away from to until the rotation is exactly the opposite direction.
 *
 * \param from first direction
 * \param to second direction
 * \param maxDegreesDelta angular step 
 * \return the transformed rotation
 */
static Quaternionf rotateTowards(Quaternionf from, Quaternionf to, float maxDegreesDelta) {
	float angle = from.angularDistance(to);
	if (angle == 0.0f) return to;

	return from.slerp(fmin(1.0f, maxDegreesDelta / angle), to);
}

 /*!
  * Projects a vector onto another vector
  *
  * \param vector the vector
  * \param onNormal the normal to project on
  * \return projected vector
  * \see https://docs.unity3d.com/ScriptReference/Vector3.Project.html
  */
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

 /*!
 * Reflects a vector off the plane defined by a plane normal
 *
 * \param vector the vector is treated as a directional arrow coming in to the plane
 * \param planeNormal the normal to project on
 * \return reflected vector
 * \see https://docs.unity3d.com/ScriptReference/Vector3.Reflect.html
 */
 static Vector3f reflect(Vector3f vector, Vector3f planeNormal) {
	 return vector - 2 * project(vector, planeNormal);
 }

 /*!
 * Projects a vector onto another vector and rejects a 
 *
 * \param a the vector to reject
 * \param b the second vector
 * \return rejected vector
 */
 static Vector3f reject(Vector3f a, Vector3f b) {
	 return a - project(a, b);
 }

 /*!
  * Normalizes normal. Normalizes tangent and makes sure it is orthogonal to normal (that is, angle between them is 90 degrees)
  *
  * \param normal the normal
  * \param tangent the tangent
  * \return angle in degrees between both vectors.
  * \see https://graemepottsfolio.wordpress.com/2015/11/26/vectors-programming/
  */
static void orthoNormalize(Vector3f& normal, Vector3f& tangent) {
	/*normal.normalize();
	tangent.normalize();

	tangent.completeOrthogonalDecomposition();

	return tangent.cross(normal);*/
	
	tangent = (tangent - project(tangent, normal.normalized())).normalized();
}

/*!
 * Normalizes normal. Normalizes tangent and makes sure it is orthogonal to normal. Normalizes binormal and makes sure it is orthogonal to both normal and tangent.
 *
 * \param normal the normal
 * \param tangent the tangent
 * \param binormal the binormal
 * \return angle in degrees between both vectors.
 * \see https://graemepottsfolio.wordpress.com/2015/11/26/vectors-programming/
 */
static void orthoNormalize(Vector3f& normal, Vector3f& tangent, Vector3f& binormal) {

	orthoNormalize(normal, tangent);

	binormal = binormal - project(binormal, normal);
	binormal = (binormal - project(binormal, tangent)).normalized();
}

/*!
 * Returns the angle in degrees between vector from and vector to. This is always the smallest
 *
 * \param from first vector.
 * \param to second vector.
 * \return angle in degrees between both vectors.
 * \see https://github.com/Unity-Technologies/UnityCsReference/blob/61f92bd79ae862c4465d35270f9d1d57befd1761/Runtime/Export/Math/Vector3.cs#L305
 */
static float angle(Vector3f from, Vector3f to) {
	// sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
	float denominator = sqrtf(from.squaredNorm() * to.squaredNorm());

	float kEpsilonNormalSqrt = 1e-15F;

	if (denominator < kEpsilonNormalSqrt) {
		return 0.0f;
	}
	float dot = clamp(from.dot(to) / denominator, -1.0f, 1.0f);

	return acosf(dot) * M_Rad2Deg;
}

/*!
 * The smaller of the two possible angles between the two vectors is returned, therefore the result will never be greater than 180 degrees or smaller than -180 degrees.
 * If you imagine the from and to vectors as lines on a piece of paper, both originating from the same point, then the /axis/ vector would point up out of the paper.
 * 
 * \param from first vector.
 * \param to second vector.
 * \param axis axis vector.
 * \return The measured angle between the two vectors. positive in a clockwise direction and negative in an anti-clockwise direction.
 * \see https://github.com/Unity-Technologies/UnityCsReference/blob/61f92bd79ae862c4465d35270f9d1d57befd1761/Runtime/Export/Math/Vector3.cs#L319
 */
static float signedAngle(const Vector3f& from, const Vector3f& to, const Vector3f& axis) {
	float unsignedAngle = angle(from, to);

	float cross_x = from.y() * to.z() - from.z() * to.y();
	float cross_y = from.z() * to.x() - from.x() * to.z();
	float cross_z = from.x() * to.y() - from.y() * to.x();

	float sign = sgn(axis.x() * cross_x + axis.y() * cross_y + axis.z() * cross_z);

	return unsignedAngle * sign;
}

/*!
 * Returns a quaternion representing a rotation around a unit axis by an angle in degree.
 * The rotation direction is clockwise when looking along the rotation axis towards the origin.
 *
 * \param angle angle of rotation in degree.
 * \param axis axis of rotation.
 * \return quaternion representing a rotation around an axis.
 * \see https://github.com/Unity-Technologies/Unity.Mathematics/blob/4915b7afebc50b9c6c9a410b7a86ae5489aa6b9c/src/Unity.Mathematics/quaternion.cs#L99
 */
static Quaternionf angleAxis(const float& angle, const Vector3f& axis) {
	// TODO: In radians umwandeln?
	float halfAngle = 0.5f * (angle * M_Deg2Rad);

	float sina = sinf(halfAngle);
	float cosa = cosf(halfAngle);

	Vector3f axisAngle = axis * sina;

	return Quaternionf(cosa, axisAngle.x(), axisAngle.y(), axisAngle.z());
}

/*!
 * Calculates the distance between vector a and vector b
 *
 * \param a first vector
 * \param b second vector
 * \return distance between both vectors
 */
static float distance(const Vector3f& a, const Vector3f& b) {
	float diff_x = a.x() - b.x();
	float diff_y = a.y() - b.y();
	float diff_z = a.z() - b.z();

	return sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
}

/*!
 * Linearly interpolates between between vector a and b by amount t (unclamped)
 *
 * \param a first vector
 * \param b second vector
 * \param t interpolant
 * \return linearly interpolated vector
 */
static Vector3f lerpUnclamped(Vector3f a, Vector3f b, float t) {
	return (b - a) * t + a;
}

/*!
 * Linearly interpolates between between vector a and b by amount t
 * The parameter t is clamped to the range [0, 1].
 *
 * \param a first vector
 * \param b second vector
 * \param t interpolant
 * \return linearly interpolated vector
 */
static Vector3f lerp(Vector3f a, Vector3f b, float t) {
	if (t <= 0) return a;
	else if (t >= 1) return b;
	return lerpUnclamped(a, b, t);
}

/*!
 * Spherically interpolates between between vector a and b by amount t (unclamped)
 *
 * \param a first vector
 * \param b second vector
 * \param t interpolant
 * \return spherically interpolated vector
 */
static Vector3f slerpUnclamped(Vector3f start, Vector3f end, float delta) {
	// Dot product - the cosine of the angle between 2 vectors
	float dot = start.dot(end);

	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = clamp(dot, -1.0f, 1.0f);

	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = acosf(dot) * delta;
	Vector3f relativeVec = (end - start * dot).normalized();

	// Orthonormal basis
	// The final result.
	return ((start * cosf(theta)) + (relativeVec * sinf(theta)));
}

/*!
 * Spherically interpolates between between vector a and b by amount t
 * The parameter t is clamped to the range [0, 1].
 * The difference between this and linear interpolation (aka, "lerp") is that the vectors are treated as directions rather than points in space.
 * 
 * \param a first vector
 * \param b second vector
 * \param t interpolant
 * \return spherically interpolated vector
 */
static Vector3f slerp(Vector3f a, Vector3f b, float t) {
	if (t <= 0) return a;
	else if (t >= 1) return b;
	return slerpUnclamped(a, b, t);
}

/*!
 * Decompose a rotation into twist components
 *
 * \param rotation twisted rotation
 * \param twistAxis axis vector
 * \return decomposed rotation
 * \see http://allenchou.net/2018/05/game-math-swing-twist-interpolation-sterp/
 */
static Quaternionf DecomposeTwist(const Quaternionf rotation, const Vector3f twistAxis) {
	// Create Euler for sqrlength check
	Vector3f r = Vector3f(rotation.x(), rotation.y(), rotation.z());

	// Singularity: rotation by 0 degree
	if (r.squaredNorm() < FLT_EPSILON) {
		return angleAxis(0, twistAxis);
	}
	else {
		// swing-twist decomposition
		Vector3f p = project(r, twistAxis);
		return Quaternionf(rotation.w(), rotation.x(), rotation.y(), rotation.z()).normalized();
	}
}

/*
static Vector3f slerpUnclamped(Vector3f a, Vector3f b, float t) {
	double magA = Magnitude(a);
	double magB = Magnitude(b);
	a /= magA;
	b /= magB;

	double dot = a.dot(b);
	dot = fmax(dot, -1.0);
	dot = fmin(dot, 1.0);

	double theta = acos(dot) * t;
	Vector3f relativeVec = (b - a * dot).normalized();
	Vector3f newVec = a * cosf(theta) + relativeVec * sinf(theta);
	return newVec * (magA + (magB - magA) * t);
}
*/

