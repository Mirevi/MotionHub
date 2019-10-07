#pragma once

#include <string>

class Vector3
{

public:

	struct position
	{
		float x, y, z;
	};

	Vector3();
	Vector3(float x, float y, float z);
	Vector3(position xyz);

	position m_xyz;

	Vector3 operator + (Vector3 const vector3);
	Vector3 operator - (Vector3 const vector3);
	Vector3 operator * (Vector3 const vector3);
	Vector3 operator / (Vector3 const vector3);
	Vector3 operator * (float const scale);
	Vector3 operator * (int const scale);

	static Vector3 zero();
	static Vector3 one();

	std::string toString();

};