#pragma once

#include "ConfigDllExportMotionHubUtil.h"
#include <osgViewer/Viewer>

class MotionHubUtil_DLL_import_export GeodeFinder : public osg::NodeVisitor
{
public:
	GeodeFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

	virtual void apply(osg::Node& node);
	osg::ref_ptr<osg::Geode> getFoundGeode();

protected:
	osg::ref_ptr<osg::Geode> m_geode;
};