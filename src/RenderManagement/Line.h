#pragma once
#include "ConfigDllExportRenderManagement.h"
#include "osg/PositionAttitudeTransform"
#include <osg/Geode>
#include <osg/geometry>
#include <osg/Vec3f>

//class ConfigManager;
//TODO: Rename to osgLine, when all branches merged again

class RenderManagement_DLL_import_export Line : public osg::PositionAttitudeTransform
{
public:
	Line(osg::ref_ptr<osg::Group> nodeToAttachTo, bool isOverlay);
    ~Line();

    void createGeometry(bool isOverlay);
	void Line::draw(osg::Vec3 start, osg::Vec3 end, osg::Vec4 colorStart, osg::Vec4 colorEnd);
	void Line::redraw();
	void clear();

private:
	float m_scale;
	float m_thickness;

	osg::ref_ptr<osg::Geode> m_geode;
	osg::ref_ptr<osg::Geometry> m_geom;
	osg::ref_ptr<osg::Vec4Array> m_colors;
	osg::ref_ptr<osg::DrawArrays> m_drawArrays;
	osg::ref_ptr<osg::Vec3Array> m_vertices;
	bool m_isDirty;
	bool m_isDisplayed;

};