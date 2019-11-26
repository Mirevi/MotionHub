#pragma once

#include "Mesh.h"

class Bone : public Mesh
{

public:
	Bone(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scalar = Vector3::one());
	~Bone();

};