#pragma once

#include "Entity.h"
#include "Cube.h"
#include "MotionHubUtil/Console.h"

class SkeletonMesh : public Entity
{

	public:
		SkeletonMesh();
		~SkeletonMesh();
		Cube* m_joints[21];

};