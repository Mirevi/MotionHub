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

		Primitive(Type type, QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scale = Vector3::one());
		~Primitive();

};