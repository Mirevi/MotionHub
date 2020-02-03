#pragma once

#include "Entity.h"
#include "Cube.h"
#include "MotionHubUtil/Console.h"

/*!
 * \class SkeletonMesh
 *
 * \brief container for joint meshes
 *
 * \author Eric Jansen
 */
class SkeletonMesh : public Entity
{

	public:
		/*!
		 * 
		 * default constructor
		 * 
		 */
		SkeletonMesh();
		~SkeletonMesh();
		/*!
		 * 
		 * joint mesh array representing all 21 skeleton joints
		 * 
		 */
		Cube* m_joints[21];

};