#include "Vector4.h"

Vector4::Vector4()
{

	m_xyzw = Vector4::zero().m_xyzw;

}

Vector4::Vector4(float x, float y, float z, float w)
{

	m_xyzw.x = x;
	m_xyzw.y = y;
	m_xyzw.z = z;
	m_xyzw.w = w;

}

Vector4::Vector4(position xyzw)
{

	m_xyzw.x = xyzw.x;
	m_xyzw.y = xyzw.y;
	m_xyzw.z = xyzw.z;
	m_xyzw.w = xyzw.w;

}

Vector4 Vector4::zero()
{

	return Vector4(0.0f, 0.0f, 0.0f, 0.0f);

}