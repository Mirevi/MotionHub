#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osg/PositionAttitudeTransform>
#include <osg/PolygonMode>
#include "KeyboardEventHandler.h"


int main(int argc, char** argv)
{
	//osg::PositionAttitudeTransform* sceneRoot = new osg::PositionAttitudeTransform();

	//osg::Vec3Array* lineVertices = new osg::Vec3Array();
	//osg::DrawElementsUInt* lineIndicies = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	//osg::Vec4Array* lineColorArray = new osg::Vec4Array();
	//osg::Geometry* geometry = new osg::Geometry();
	//osg::Geode* geode = new osg::Geode();

	////Use VBOs
	//geometry->setUseDisplayList(false);
	//geometry->setUseVertexBufferObjects(true);
	//geometry->setDataVariance(osg::Object::DYNAMIC);

	////Define Vertex Positions
	//osg::Vec3f startPoint(0, 0, 0);
	//osg::Vec3f joint1(1, 0, 1);
	//osg::Vec3f joint2(2, 0, 2);
	//osg::Vec3f endPoint(3, 0, 1);

	////Put Positions to VertexArray
	//lineVertices->push_back(startPoint);
	//lineVertices->push_back(joint1);
	//lineVertices->push_back(joint2);
	//lineVertices->push_back(endPoint);

	////Create indicies for lines
	//lineIndicies->push_back(0);
	//lineIndicies->push_back(1);
	//lineIndicies->push_back(1);
	//lineIndicies->push_back(2);
	//lineIndicies->push_back(2);
	//lineIndicies->push_back(3);

	//lineColorArray->push_back(osg::Vec4f(1.0, 0.0, 0.0, 1.0));

	//geometry->setVertexArray(lineVertices);
	//geometry->setColorArray(lineColorArray);
	//geometry->addPrimitiveSet(lineIndicies);

	//sceneRoot->addChild(geode);
	//geode->addDrawable(geometry);

	//osgViewer::Viewer* viewer = new osgViewer::Viewer();
	//viewer->setSceneData(sceneRoot);
	//viewer->setCameraManipulator(new osgGA::TrackballManipulator());

	//float* value = new float();
	//*value = 1.0f;

	//KeyboardEventHandler* kbEventHandler = new KeyboardEventHandler(value);
	//viewer->addEventHandler(kbEventHandler);
	//
	//while (!viewer->done())
	//{
	//	viewer->frame();
	//}

	std::cout << "Hello World" << std::endl;
	std::cin.get();

	return 0;

}
