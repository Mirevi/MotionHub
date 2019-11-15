#pragma once

#include "Mesh.h"

class Plane : public Mesh
{

	public:
		Plane(QOpenGLTexture* texture);
		~Plane();

};