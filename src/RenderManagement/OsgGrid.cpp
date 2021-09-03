// ClayMore - Immersive Mesh Modelling --- Copyright (c) 2014-2017 Philipp Ladwig, Jannik Fiedler, Jan Beutgen
#include "OsgGrid.h"

#include <osg/LineWidth>

OsgGrid::OsgGrid(unsigned short lineCount, GridMode mode, float scaleIn, float lineWidthInRGBAxis, float lineWidthGreyAxis) :
	m_mode(mode)
{
	m_geometry = NULL;
	m_geometryAxes = NULL;

	m_matrixTransform = new osg::MatrixTransform();
	m_matrixTransform->setMatrix(osg::Matrix::scale(scaleIn, scaleIn, scaleIn));

	initialize(lineCount, lineWidthInRGBAxis, lineWidthGreyAxis);
}

OsgGrid::~OsgGrid()
{
}

void OsgGrid::initialize(int lineCount, float lineWidthInRGBAxis, float lineWidthGreyAxis)
{
	//Axes (color-coded)
	osg::ref_ptr<osg::Vec3Array> pAxesVertexArray = new osg::Vec3Array();
	pAxesVertexArray->push_back(osg::Vec3(lineCount * 8, 0, 0));
	pAxesVertexArray->push_back(osg::Vec3(-lineCount * 8, 0, 0));
	if (m_mode == SHOW_ALL)
	{
		pAxesVertexArray->push_back(osg::Vec3(0, lineCount * 8, 0));
		pAxesVertexArray->push_back(osg::Vec3(0, -lineCount * 8, 0));
	}
	pAxesVertexArray->push_back(osg::Vec3(0, 0, lineCount * 8));
	pAxesVertexArray->push_back(osg::Vec3(0, 0, -lineCount * 8));

	osg::ref_ptr<osg::DrawElementsUShort> pAxesDrawElementsUShort = new osg::DrawElementsUShort(osg::PrimitiveSet::LINES);
	pAxesDrawElementsUShort->push_back(0);
	pAxesDrawElementsUShort->push_back(1);

	pAxesDrawElementsUShort->push_back(2);
	pAxesDrawElementsUShort->push_back(3);

	if (m_mode == SHOW_ALL)
	{
		pAxesDrawElementsUShort->push_back(4);
		pAxesDrawElementsUShort->push_back(5);
	}

	osg::ref_ptr<osg::Vec4Array> pAxesColorArray = new osg::Vec4Array;
	pAxesColorArray->push_back(osg::Vec4f(1.0f, 0.0f, 0.0f, 0.3f));
	pAxesColorArray->push_back(osg::Vec4f(1.0f, 0.0f, 0.0f, 0.3f));
	if (m_mode == SHOW_ALL)
	{
		pAxesColorArray->push_back(osg::Vec4f(0.0f, 1.0f, 0.0f, 0.3f));
		pAxesColorArray->push_back(osg::Vec4f(0.0f, 1.0f, 0.0f, 0.3f));
	}
	pAxesColorArray->push_back(osg::Vec4f(0.0f, 0.0f, 1.0f, 0.3f));
	pAxesColorArray->push_back(osg::Vec4f(0.0f, 0.0f, 1.0f, 0.3f));

	m_geometryAxes = new osg::Geometry();
	m_geometryAxes->setVertexArray(pAxesVertexArray);
	m_geometryAxes->addPrimitiveSet(pAxesDrawElementsUShort);
	m_geometryAxes->setColorArray(pAxesColorArray);
	m_geometryAxes->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//Use VBO, instead of DisplayLists
	m_geometryAxes->setUseDisplayList(false);
	m_geometryAxes->setUseVertexBufferObjects(true);
	m_geometryAxes->setDataVariance(osg::Object::STATIC);

	osg::LineWidth* lineWidthRGB = new osg::LineWidth();
	lineWidthRGB->setWidth(lineWidthInRGBAxis);
	m_geometryAxes->getOrCreateStateSet()->setAttributeAndModes(lineWidthRGB, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	m_geometryAxes->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);


	//Other (grey)
	osg::ref_ptr<osg::Vec3Array> pVertexArray = new osg::Vec3Array();
	
	osg::ref_ptr<osg::DrawElementsUShort> pDrawElementsUShort = new osg::DrawElementsUShort(osg::PrimitiveSet::LINES);

	int primitiveSetCounter = 0;
	int lineCountHalf = lineCount / 2;
	int lineCount2 = lineCount * 2;
	int lineCount4 = lineCount * 4;
	int lineCount8 = lineCount * 8;
	int lineCount16 = lineCount * 16;
	float yPos = 0.001f;
	for (int i = -lineCountHalf; i <= lineCountHalf; ++i)
	{
		pVertexArray->push_back(osg::Vec3(-lineCountHalf, yPos, i));
		pVertexArray->push_back(osg::Vec3(lineCountHalf, yPos, i));
		pVertexArray->push_back(osg::Vec3(i, yPos, -lineCountHalf));
		pVertexArray->push_back(osg::Vec3(i, yPos, lineCountHalf));

		pDrawElementsUShort->push_back(primitiveSetCounter++);
		pDrawElementsUShort->push_back(primitiveSetCounter++);
		pDrawElementsUShort->push_back(primitiveSetCounter++);
		pDrawElementsUShort->push_back(primitiveSetCounter++);
	}

	//for (int i = -lineCount; i <= lineCount; i += 2)
	//{
	//	pVertexArray->push_back(osg::Vec3(-lineCount, yPos-0.01f, i));
	//	pVertexArray->push_back(osg::Vec3(lineCount, yPos - 0.01f, i));
	//	pVertexArray->push_back(osg::Vec3(i, yPos - 0.01f, -lineCount));
	//	pVertexArray->push_back(osg::Vec3(i, yPos - 0.01f, lineCount));

	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//}

	//for (int i = -lineCount2; i <= lineCount2; i += 4)
	//{
	//	pVertexArray->push_back(osg::Vec3(-lineCount2, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(lineCount2, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, -lineCount2));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, lineCount2));

	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//}

	//for (int i = -lineCount4; i <= lineCount4; i += 8)
	//{
	//	pVertexArray->push_back(osg::Vec3(-lineCount4, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(lineCount4, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, -lineCount4));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, lineCount4));

	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//}

	//for (int i = -lineCount8; i <= lineCount8; i += 16)
	//{
	//	pVertexArray->push_back(osg::Vec3(-lineCount8, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(lineCount8, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, -lineCount8));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, lineCount8));

	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//}

	//for (int i = -lineCount16; i <= lineCount16; i += 32)
	//{
	//	pVertexArray->push_back(osg::Vec3(-lineCount16, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(lineCount16, yPos, i));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, -lineCount16));
	//	pVertexArray->push_back(osg::Vec3(i, yPos, lineCount16));

	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//	pDrawElementsUShort->push_back(primitiveSetCounter++);
	//}
	
	osg::ref_ptr<osg::Vec4Array> pColorArray = new osg::Vec4Array;
	pColorArray->push_back(osg::Vec4f(0.5f,0.5f,0.5f,0.3f));

	m_geometry = new osg::Geometry();
	m_geometry->setVertexArray(pVertexArray);
	m_geometry->addPrimitiveSet(pDrawElementsUShort);
	m_geometry->setColorArray(pColorArray);
	m_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::LineWidth* lineWidthGrey = new osg::LineWidth();
	lineWidthGrey->setWidth(lineWidthGreyAxis);
	m_geometry->getOrCreateStateSet()->setAttributeAndModes(lineWidthGrey, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	m_geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_geometry->computeBound();

	//Geode
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(m_geometryAxes);
	geode->addDrawable(m_geometry);
	geode->computeBound();

	osg::ref_ptr<osg::StateSet> pGeodeStateSet = geode->getOrCreateStateSet();

	osg::PolygonMode * polygonMode = new osg::PolygonMode;
	polygonMode->setMode(osg::PolygonMode::FRONT, osg::PolygonMode::LINE);

	pGeodeStateSet->setAttributeAndModes(polygonMode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	pGeodeStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	pGeodeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

	m_matrixTransform->addChild(geode);
}
