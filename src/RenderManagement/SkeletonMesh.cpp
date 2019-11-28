#include "SkeletonMesh.h"

SkeletonMesh::SkeletonMesh()
{

	Console::log("SkeletonMesh::SkeletonMesh(): Created skeleton mesh.");

	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
		m_joints[indexJoint] = new Cube();

}

SkeletonMesh::~SkeletonMesh()
{



}