#pragma once
#include "ConfigDllExportRenderManagement.h"
#include <osg/PositionAttitudeTransform>
#include "MotionHubUtil/Joint.h"
#include <osg/Node>

class OsgLine;

//TODO: Check if derivation from Node is necessary
class RenderManagement_DLL_import_export OsgBone : public osg::Node
{

public:
    //Bone, connected with two known joints. By default, an identity quaternion ("no rotation") is set
	OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint,
        osg::ref_ptr<osg::PositionAttitudeTransform> endJoint,
        osg::ref_ptr<osg::Group> stickManGroup,
        bool isStickManRenderingActive,
        osg::Quat rotationOffset = osg::Quat(0, 0, 0, 1));

    //Leaf Bone, connected with only one joint. By default, an identity quaternion ("no rotation") is set
	OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint,
        osg::ref_ptr<osg::Group> stickManGroup,
        bool isStickManRenderingActive,
        osg::Quat rotationOffset = osg::Quat(0, 0, 0, 1));
    
    ~OsgBone();

    void setStartJoint(Joint joint);
    void setEndJoint(Joint joint);
    void setOffsetRotation(osg::Vec3 offset);
    void update();
    void updateScale(float scale);
	void createLeafJoint();
    void updateStickManRenderingState(bool renderStickMan);

private:
    void initialize(osg::ref_ptr<osg::Group> stickManGroup, osg::Quat rotationOffset);
    //osg::ref_ptr<osg::Group> m_parentNode;

    osg::ref_ptr<osg::Node> m_boneNode;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_startJoint;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_startJointOffset;
    osg::ref_ptr<osg::PositionAttitudeTransform> m_endJoint;
    //osg::ref_ptr<osg::PositionAttitudeTransform> m_endJointOffset; //TODO: Necessary?
    
    //Offset rotation to correctly render the bone
    osg::Vec3f offsetRotation;

    //bool isLeafJoint;

    OsgLine* m_line;
    bool m_isStickManRenderingActive;
    bool m_isGlLookAtSolidBoneRotationActivated;

};