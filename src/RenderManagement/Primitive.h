#pragma once

#include "Mesh.h"

class Primitive : public Mesh
{

	public:

		enum Type
		{
			Plane,
			Cube
		};

		Primitive(Type type, QOpenGLTexture* texture, Vector3 position = Vector3::zero());
		~Primitive();


};