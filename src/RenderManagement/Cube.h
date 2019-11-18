#pragma once

#include "Mesh.h"

class Cube : public Mesh
{

public:
	Cube(QOpenGLTexture* texture, Vector3 position = Vector3::zero());
	~Cube();

};