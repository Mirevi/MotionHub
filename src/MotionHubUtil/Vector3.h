#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <string>
#include "MMHmath.h"


/*!
 * \class Vector3
 *
 * \brief Utility Class
 *
 * \author Eric Jansen
 */
class MotionHubUtil_DLL_import_export Vector3
{

public:

	struct Position
	{
		float x, y, z;
	};

	Vector3();
	Vector3(float x, float y, float z);
	Vector3(Position xyz);
	Vector3(Vector3f vector);
	Vector3(Vector4f vector);

	Position m_xyz;

	Vector3 operator + (Vector3 const vector3);
	Vector3 operator - (Vector3 const vector3);
	Vector3 operator * (Vector3 const vector3);
	Vector3 operator / (Vector3 const vector3);
	Vector3 operator * (float const scale);
	Vector3 operator * (int const scale);

	static Vector3 zero();
	static Vector3 one();

	std::string toString();

	Vector3f toEigen();

};