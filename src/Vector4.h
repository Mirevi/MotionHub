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

};