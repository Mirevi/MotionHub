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

Vector4::Vector4(Position xyzw)
{

	m_xyzw.x = xyzw.x;
	m_xyzw.y = xyzw.y;
	m_xyzw.z = xyzw.z;
	m_xyzw.w = xyzw.w;

}

Vector4::Vector4(Vector4f vector)
{

	m_xyzw.x = vector.x();
	m_xyzw.y = vector.y();
	m_xyzw.z = vector.z();
	m_xyzw.w = vector.w();

}

Vector4::Vector4(Quaternionf quaternion)
{

	m_xyzw.x = quaternion.x();
	m_xyzw.y = quaternion.y();
	m_xyzw.z = quaternion.z();
	m_xyzw.w = quaternion.w();

}


Vector4 Vector4::operator + (Vector4 const vector4)
{

	return Vector4(m_xyzw.x + vector4.m_xyzw.x, m_xyzw.y + vector4.m_xyzw.y, m_xyzw.z + vector4.m_xyzw.z, m_xyzw.w + vector4.m_xyzw.w);

};

Vector4 Vector4::operator - (Vector4 const vector4)
{

	return Vector4(m_xyzw.x - vector4.m_xyzw.x, m_xyzw.y - vector4.m_xyzw.y, m_xyzw.z - vector4.m_xyzw.z, m_xyzw.w - vector4.m_xyzw.w);

};

Vector4 Vector4::operator * (Vector4 const vector4)
{

	return Vector4(m_xyzw.x * vector4.m_xyzw.x, m_xyzw.y * vector4.m_xyzw.y, m_xyzw.z * vector4.m_xyzw.z, m_xyzw.w * vector4.m_xyzw.w);

};

Vector4 Vector4::operator / (Vector4 const vector4)
{

	return Vector4(m_xyzw.x / vector4.m_xyzw.x, m_xyzw.y / vector4.m_xyzw.y, m_xyzw.z / vector4.m_xyzw.z, m_xyzw.w / vector4.m_xyzw.w);

};

Vector4 Vector4::operator * (float const scale)
{

	return Vector4(m_xyzw.x * scale, m_xyzw.y * scale, m_xyzw.z * scale, m_xyzw.w * scale);

};

Vector4 Vector4::operator * (int const scale)
{

	return Vector4(m_xyzw.x * scale, m_xyzw.y * scale, m_xyzw.z * scale, m_xyzw.w * scale);

};

Vector4 Vector4::zero()
{

	return Vector4(0.0f, 0.0f, 0.0f, 0.0f);

}

Vector4 Vector4::one()
{

	return Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}

std::string Vector4::toString()
{

	return "(" + std::to_string(m_xyzw.x) + ", " + std::to_string(m_xyzw.y) + ", " + std::to_string(m_xyzw.z) + ", " + std::to_string(m_xyzw.w) + ")";

}