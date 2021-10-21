#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Skeleton.h"
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include "OsgSphere.h"

class OsgLine;
class OsgBone;
class OsgAxesCross;
class OsgSphere;

class RenderManagement_DLL_import_export OsgSkeleton
{

public:
	OsgSkeleton(osg::ref_ptr<osg::Group> parentNode);
	~OsgSkeleton();

	void update(Skeleton skeleton);
	void removeAndDelete();

	void toggleJointAxes(bool menuValue);
	void toggleStickManRendering(bool menuValue);
	void toggleSolidBoneRendering(bool menuValue);
	void toggleConfidenceSpheresRendering(bool menuValue);

	/*!
	 * getter for skeleton ID
	 *
	 * \return m_sid
	 */
	int getSid();

private:
	 /*!
	 * skeleton ID from skeleton received in update(Skeleton skeleton)
	 *
	 */
	int m_sid;

	osg::ref_ptr<osg::Group> m_parentNode;
	osg::ref_ptr<osg::Group> m_skeletonRootNode;
	osg::ref_ptr<osg::Group> m_boneGroup;
	osg::ref_ptr<osg::Group> m_confidenceSpheresGroup;
	std::vector<osg::ref_ptr<osg::PositionAttitudeTransform>> m_joints;
	std::vector<OsgBone*> m_bones;

	bool m_toggleConfidenceSpheres;
	std::vector<OsgSphere*> m_confidenceSpheres;
	osg::Vec4f m_colorRed, m_colorYellow, m_colorGreen;

	bool m_toggleSolidBoneRendering;

	bool m_toggleJointAxes;
	osg::ref_ptr<osg::Group> m_axesCrossGroup;
	std::vector<OsgAxesCross*> m_axesCrosses;

	bool m_toggleStickManRendering;
	osg::ref_ptr<osg::Group> m_stickManGroup;

	short m_consoleOutputModuloDelay;

};