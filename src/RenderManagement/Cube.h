#pragma once

#include "Mesh.h"

class Cube : public Mesh
{

public:
	Cube();
	Cube(QOpenGLShaderProgram* shaderProgram, QOpenGLTexture* texture, Vector3 position = Vector3::zero(), Vector3 scale = Vector3::one());
	~Cube();

private:
	void initVerts();

};