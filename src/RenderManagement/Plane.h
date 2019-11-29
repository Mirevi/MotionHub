#pragma once

#include "Mesh.h"

class Plane : public Mesh
{

public:
	Plane(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scale = Vector3::one());
	~Plane();

};