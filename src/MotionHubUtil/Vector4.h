#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <string>

/*!
 * \class Vector4
 *
 * \brief Utility Class
 *
 * \author Eric Jansen
 */
class MotionHubUtil_DLL_import_export Vector4
{

public:

	struct Position
	{
		float x, y, z, w;
	};

	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(Position xyzw);

	Position m_xyzw;

	Vector4 operator + (Vector4 const vector4);
	Vector4 operator - (Vector4 const vector4);
	Vector4 operator * (Vector4 const vector4);
	Vector4 operator / (Vector4 const vector4);
	Vector4 operator * (float const scale);
	Vector4 operator * (int const scale);

	static Vector4 zero();
	static Vector4 one();

	std::string toString();

};