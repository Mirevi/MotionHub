#pragma once
#include "ConfigDllExportRenderManagement.h"
#include <osg/PositionAttitudeTransform>
#include "MotionHubUtil/Joint.h"
//#include <osg/Geode>
//#include <osg/geometry>
//#include <osg/Vec3f>


class RenderManagement_DLL_import_export OsgBone : public osg::PositionAttitudeTransform
{
public:
	OsgBone(osg::ref_ptr<osg::Group> parentNode);
    ~OsgBone();

    void setStartJoint(Joint joint);
    void setEndJoint(Joint joint);
    void setOffsetRotation(osg::Vec3 offset);
    void updateScale(float scale);
	void createLeafJoint();

private:
    osg::ref_ptr<osg::Group> m_parentNode;

    Joint m_startJoint;
    Joint m_endJoint;
    
    //Offset rotation to correctly render the bone
    osg::Vec3f offsetRotation;

    bool hasLeafJoint;


};