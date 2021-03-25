#include "Vector3.h"

Vector3::Vector3()
{

	m_xyz = Vector3::zero().m_xyz;

}

Vector3::Vector3(float x, float y, float z)
{

	m_xyz.x = x;
	m_xyz.y = y;
	m_xyz.z = z;

}

Vector3::Vector3(Position xyz)
{

	m_xyz.x = xyz.x;
	m_xyz.y = xyz.y;
	m_xyz.z = xyz.z;

}

Vector3::Vector3(Vector3f vector)
{

	m_xyz.x = vector.x();
	m_xyz.y = vector.y();
	m_xyz.z = vector.z();

}

Vector3::Vector3(Vector4f vector)
{

	m_xyz.x = vector.x();
	m_xyz.y = vector.y();
	m_xyz.z = vector.z();

}


Vector3 Vector3::zero()
{

	return Vector3(0.0f, 0.0f, 0.0f);

}

Vector3 Vector3::one()
{

	return Vector3(1.0f, 1.0f, 1.0f);

}

Vector3 Vector3::operator + (Vector3 const vector3)
{

	return Vector3(m_xyz.x + vector3.m_xyz.x, m_xyz.y + vector3.m_xyz.y, m_xyz.z + vector3.m_xyz.z);

}

Vector3 Vector3::operator - (Vector3 const vector3)
{

	return Vector3(m_xyz.x - vector3.m_xyz.x, m_xyz.y - vector3.m_xyz.y, m_xyz.z - vector3.m_xyz.z);

}

Vector3 Vector3::operator * (Vector3 const vector3)
{

	return Vector3(m_xyz.x * vector3.m_xyz.x, m_xyz.y * vector3.m_xyz.y, m_xyz.z * vector3.m_xyz.z);

}

Vector3 Vector3::operator / (Vector3 const vector3)
{

	return Vector3(m_xyz.x / vector3.m_xyz.x, m_xyz.y / vector3.m_xyz.y, m_xyz.z / vector3.m_xyz.z);

}

Vector3 Vector3::operator * (float const scale)
{

	return Vector3(m_xyz.x * scale, m_xyz.y * scale, m_xyz.z * scale);

}

Vector3 Vector3::operator * (int const scale)
{

	return Vector3(m_xyz.x * scale, m_xyz.y * scale, m_xyz.z * scale);

}

std::string Vector3::toString()
{

	return "(" + std::to_string(m_xyz.x) + ", " + std::to_string(m_xyz.y) + ", " + std::to_string(m_xyz.z) + ")";

}

Vector3f Vector3::toEigen()
{

	return Vector3f(m_xyz.x, m_xyz.y, m_xyz.z);

}

bool Vector3::operator == (Vector3 const vector3)
{
	if (this == &vector3)
		return true;

	if (m_xyz.x == vector3.m_xyz.x && m_xyz.y == vector3.m_xyz.y && m_xyz.z == vector3.m_xyz.z)
		return true;

	return false;

}

bool Vector3::operator != (Vector3 const vector3)
{

	return !(*this == vector3);

}