#pragma once
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Skeleton.h"
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

//TODO: Rename to osgSkeleton, when all branches merged again

/*!
 * \class SkeletonMesh
 *
 * \brief container for joint meshes
 *
 * \author Eric Jansen
 */

class OsgLine;

//TODO Derive from PostAtt or Group?
class OsgSkeleton
{

public:
	/*!
	 *
	 * default constructor
	 *
	 */
	OsgSkeleton(osg::ref_ptr<osg::Group> rootNode);
	~OsgSkeleton();
	
	void update(Skeleton skeleton);


	/*!
	 *
	 * joint mesh array representing all 21 skeleton joints
	 *
	 */
private:
	osg::ref_ptr<osg::Group> m_externalRootNode;
	osg::ref_ptr<osg::Group> m_skeletonRootNode;
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> m_joints;
	std::vector<osg::ref_ptr<osg::Node>> m_bones;

	//juj löschen:
	OsgLine* m_line;

	osg::PositionAttitudeTransform* m_test;


};