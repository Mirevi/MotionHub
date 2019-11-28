#pragma once

#include "Entity.h"
#include "Cube.h"

class SkeletonMesh : public Entity
{

	public:
		SkeletonMesh();
		~SkeletonMesh();
		Cube* m_joints[21];

};