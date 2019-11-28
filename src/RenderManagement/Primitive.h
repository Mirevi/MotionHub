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

		Primitive(Type type, QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3f position = Vector3f(0, 0, 0), Vector3f scale = Vector3f(1, 1, 1));
		~Primitive();

};