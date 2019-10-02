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

Vector3::Vector3(position xyz)
{

	m_xyz.x = xyz.x;
	m_xyz.y = xyz.y;
	m_xyz.z = xyz.z;

}

Vector3 Vector3::zero()
{

	return Vector3(0.0f, 0.0f, 0.0f);

}