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

	Vector4 static zero();

	Vector4 operator + (Vector4 const vector4)
	{

		return Vector4(m_xyzw.x + vector4.m_xyzw.x, m_xyzw.y + vector4.m_xyzw.y, m_xyzw.z + vector4.m_xyzw.z, m_xyzw.w + vector4.m_xyzw.w);

	};

	Vector4 operator - (Vector4 const vector4)
	{

		return Vector4(m_xyzw.x - vector4.m_xyzw.x, m_xyzw.y - vector4.m_xyzw.y, m_xyzw.z - vector4.m_xyzw.z, m_xyzw.w - vector4.m_xyzw.w);

	};

	Vector4 operator * (Vector4 const vector4)
	{

		return Vector4(m_xyzw.x * vector4.m_xyzw.x, m_xyzw.y * vector4.m_xyzw.y, m_xyzw.z * vector4.m_xyzw.z, m_xyzw.w * vector4.m_xyzw.w);

	};

	Vector4 operator / (Vector4 const vector4)
	{

		return Vector4(m_xyzw.x / vector4.m_xyzw.x, m_xyzw.y / vector4.m_xyzw.y, m_xyzw.z / vector4.m_xyzw.z, m_xyzw.w / vector4.m_xyzw.w);

	};
};