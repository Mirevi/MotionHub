#include "SkeletonMesh.h"

SkeletonMesh::SkeletonMesh()
{

	// create 21 joints
	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
	{

		m_joints[indexJoint] = new Cube();

	}
}

SkeletonMesh::~SkeletonMesh()
{



}