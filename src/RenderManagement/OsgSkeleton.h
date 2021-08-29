#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Skeleton.h"
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

class OsgLine;
class OsgBone;
class OsgAxesCross;

class RenderManagement_DLL_import_export OsgSkeleton
{

public:
	/*!
	 *
	 * default constructor
	 *
	 */
	OsgSkeleton(osg::ref_ptr<osg::Group> parentNode);
	~OsgSkeleton();
	
	void update(Skeleton skeleton);
	void removeAndDelete();

	void toggleJointAxes(bool menuValue);
	void toggleStickManRendering(bool menuValue);
	void toggleSolidBoneRendering(bool menuValue);


	/*!
	 *
	 * joint mesh array representing all 21 skeleton joints
	 *
	 */
private:
	osg::ref_ptr<osg::Group> m_parentNode;
	osg::ref_ptr<osg::Group> m_skeletonRootNode;
	osg::ref_ptr<osg::Group> m_boneGroup;
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> m_joints;
	std::vector<OsgBone*> m_bones;
	
	bool m_toggleSolidBoneRendering;
	
	bool m_toggleJointAxes;
	osg::ref_ptr<osg::Group> m_axesCrossGroup;
	std::vector<OsgAxesCross*> m_axesCrosses;

	bool m_toggleStickManRendering;
	osg::ref_ptr<osg::Group> m_stickManGroup;

	short m_consoleOutputModuloDelay;

};