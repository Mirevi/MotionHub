#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "osg/PositionAttitudeTransform"
#include <osg/ShapeDrawable>


class RenderManagement_DLL_import_export OsgSphere : public osg::PositionAttitudeTransform
{
public:
	OsgSphere();
	OsgSphere(osg::ref_ptr<osg::Group> nodeToAttachTo);
    ~OsgSphere();

    void createGeometry();

	void setScale(int scale); //TODO4: Implement scaling
	int getScale();

	osg::ShapeDrawable* getSphereGeometry();

private:
	void initialize();
	osg::ref_ptr<osg::ShapeDrawable> m_sphere;
	float m_scale;

};