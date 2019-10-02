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

};