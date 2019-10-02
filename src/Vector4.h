#pragma once

#include <string>

class Vector4
{

public:

	struct position
	{
		float x, y, z, w;
	};

	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(position xyzw);

	position m_xyzw;

	Vector4 operator + (Vector4 const vector4);
	Vector4 operator - (Vector4 const vector4);
	Vector4 operator * (Vector4 const vector4);
	Vector4 operator / (Vector4 const vector4);
	Vector4 operator * (float const scale);
	Vector4 operator * (int const scale);

	Vector4 static zero();
	Vector4 static one();

	std::string toString();

};