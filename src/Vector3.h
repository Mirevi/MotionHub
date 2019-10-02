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

	Vector3 static zero();

	Vector3 operator + (Vector3 const vector3)
	{

		return Vector3(m_xyz.x + vector3.m_xyz.x, m_xyz.y + vector3.m_xyz.y, m_xyz.z + vector3.m_xyz.z);

	};

	Vector3 operator - (Vector3 const vector3)
	{

		return Vector3(m_xyz.x - vector3.m_xyz.x, m_xyz.y - vector3.m_xyz.y, m_xyz.z - vector3.m_xyz.z);

	};

	Vector3 operator * (Vector3 const vector3)
	{

		return Vector3(m_xyz.x * vector3.m_xyz.x, m_xyz.y * vector3.m_xyz.y, m_xyz.z * vector3.m_xyz.z);

	};

	Vector3 operator / (Vector3 const vector3)
	{

		return Vector3(m_xyz.x / vector3.m_xyz.x, m_xyz.y / vector3.m_xyz.y, m_xyz.z / vector3.m_xyz.z);

	};
};