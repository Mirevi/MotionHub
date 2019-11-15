#pragma once

#include "Mesh.h"

class Cube : public Mesh
{

public:
	Cube(QOpenGLTexture* texture);
	~Cube();

};