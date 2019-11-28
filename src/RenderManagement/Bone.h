#pragma once

#include "Mesh.h"

class Bone : public Mesh
{

public:
	Bone(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3f position = Vector3f(0, 0, 0), Vector3f scalar = Vector3f(1, 1, 1));
	~Bone();

};