#include "OsgLine.h"
//#include "MotionHubUtil/ConfigManager.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/LineWidth>

OsgLine::OsgLine(osg::ref_ptr<osg::Group> nodeToAttachTo, bool isOverlay) : m_isDirty{ false }, m_isDisplayed{ true } {

	nodeToAttachTo->addChild(this);
	m_scale = 2.0;

	createGeometry(isOverlay);
	
}

void OsgLine::createGeometry(bool isOverlay)
{
	//Setup  geometry to draw lines
	m_geode = new osg::Geode();
	m_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF); //No shading -> no light
	m_geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	if (isOverlay) m_geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	m_geom = new osg::Geometry();
	m_geom->setUseDisplayList(false);
	m_geom->setUseVertexBufferObjects(true);
	m_geom->setDataVariance(osg::Object::DYNAMIC);
	m_drawArrays = new osg::DrawArrays(osg::PrimitiveSet::LINES);
	m_geom->addPrimitiveSet(m_drawArrays);
	m_colors = new osg::Vec4Array();
	m_colors->setBinding(osg::Array::BIND_PER_VERTEX);
	m_geom->setColorArray(m_colors);
	m_vertices = new osg::Vec3Array();
	m_geom->setVertexArray(m_vertices);
	m_geode->addDrawable(m_geom);

	osg::LineWidth* lineWidth = new osg::LineWidth();
	lineWidth->setWidth(m_scale);
	m_geode->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

	addChild(m_geode);
}

void OsgLine::draw(osg::Vec3 start, osg::Vec3 end, osg::Vec4 colorStart, osg::Vec4 colorEnd)
{
	m_vertices->push_back(start);
	m_vertices->push_back(end);
	m_colors->push_back(colorStart);
	m_colors->push_back(colorEnd);
	m_isDirty = true;
}

void OsgLine::redraw()
{
	if (!m_isDirty) return;
	m_drawArrays->setFirst(0);
	m_drawArrays->setCount(m_vertices->size());
	m_geom->dirtyBound();
	m_geom->dirtyGLObjects();
	m_vertices->dirty();
	m_colors->dirty();
	m_drawArrays->dirty();
	m_isDirty = false;
}


void OsgLine::clear()
{
	if(m_vertices)
	{
	m_vertices->clear();
	}

	if(m_colors)
	{
		m_colors->clear();
	}
	
}


OsgLine::~OsgLine()
{

}