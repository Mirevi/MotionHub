#include "OsgBone.h"
#include <osgDB/ReadFile>
#include "OsgLine.h"
#include <MotionHubUtil/GeodeFinder.h>

//TODO: Check if the standard CTOR is necessary
OsgBone::OsgBone()
{
}


//CTOR for start and end joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, osg::ref_ptr<osg::PositionAttitudeTransform> endJoint,
	osg::ref_ptr<osg::Group> stickManGroup, osg::Quat rotationOffset)
	: m_startJoint(startJoint), m_endJoint(endJoint)
{
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);
	m_line = new OsgLine(stickManGroup, true);
	m_toggleStickManRendering = true;
	

	//Attempt to create a visualization for the confidence values
	//I'll commit it, but I won't use it. I think, colored spheres a better indicators than bone transition colors
	GeodeFinder geodeFinder;
	m_boneNode->accept(geodeFinder);
	osg::ref_ptr<osg::Geode> geode = geodeFinder.getFoundGeode();
	
	osg::Vec4Array* colorArray = new osg::Vec4Array();
	colorArray->setBinding(osg::Array::BIND_PER_VERTEX);
	geode->getDrawable(0)->asGeometry()->setColorArray(colorArray);

	osg::Vec3Array* vertexArray = new osg::Vec3Array();
	vertexArray = dynamic_cast<osg::Vec3Array*>(geode->getDrawable(0)->asGeometry()->getVertexArray());


	for (int i = 0; i < geode->getDrawable(0)->asGeometry()->getVertexArray()->getNumElements(); i++)
	{
		colorArray->push_back(osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	}
	
	//These verts is the lowest vertexin the bone.obj. The OSGReader splits this single vert into 4 for 4 corresponding faces
	colorArray->at(144) = osg::Vec4f(1.0f, -0.5f, -0.5f, 1.0f);
	colorArray->at(147) = osg::Vec4f(1.0f, -0.5f, -0.5f, 1.0f);
	colorArray->at(150) = osg::Vec4f(1.0f, -0.5f, -0.5f, 1.0f);
	colorArray->at(150) = osg::Vec4f(1.0f, -0.5f, -0.5f, 1.0f);

}


//CTOR for leaf joint with only one joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint, float lengthToVirtualEndJoint, osg::ref_ptr<osg::Group> stickManGroup,
	osg::Quat rotationOffset)
	: m_startJoint(startJoint)
{
	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);
	m_line = new OsgLine(stickManGroup, true);
	m_toggleStickManRendering = true;

	// m_endJoint -> To be defined?
}


OsgBone::~OsgBone()
{
	//Every member is a osg::ref_ptr and should be cleaned by OSG gabarage collector
	//std::cout << "DTOR OsgBone" << std::endl;
}


void OsgBone::setStartJoint(Joint joint)
{
	//m_startJoint = joint;
}

void OsgBone::setEndJoint(Joint joint)
{
	//m_endJoint = joint;
}

void OsgBone::setOffsetRotation(osg::Vec3 offset)
{
}

void OsgBone::update()
{
	//Check if a leaf bone, then do not use the endJoint, because it is not initialized
	if (m_endJoint)
	{
		osg::Vec3f boneVector = m_startJoint->getPosition() - m_endJoint->getPosition();
		float boneLength = boneVector.length();
		m_startJointOffset->setScale(osg::Vec3(boneLength, boneLength, boneLength));

		if (m_toggleStickManRendering)
		{
			m_line->clear();
			// create a  line
			m_line->draw(m_startJoint->getPosition(), m_endJoint->getPosition(), osg::Vec4f(1.0, 0.0, 0.0, 1.0), osg::Vec4f(0.0, 1.0, 0.0, 1.0));
			// to reset the line count to 0, use clear(). After this, Line is empty and new lines can be added
			m_line->redraw();
		}
		else
		{
			m_line->clear();
			m_line->redraw();
		}
	}
	else
	{
		m_startJointOffset->setScale(osg::Vec3(0.1f, 0.1f, 0.1f));
		//############################# DO LINE RENDERING??????????????
		//############################# DO LINE RENDERING??????????????
		//############################# DO LINE RENDERING??????????????
	}


	//Test for rotation
	//m_startJointOffset;


	//osg::Vec3f dir = osg::Vec3f(0.0, 0.0, 50.0) - m_startJointOffset->getPosition();
	//dir = m_startJointOffset->getAttitude() * dir;
	//osg::Vec3f lineEndPosition = dir + m_startJointOffset->getPosition();

	//osg::Matrix matrix;
	//matrix.makeLookAt(m_startJointOffset->getPosition(), lineEndPosition, osg::Y_AXIS);
	//m_startJointOffset->setAttitude(matrix.getRotate());
}

void OsgBone::updateScale(float scale)
{
	m_startJointOffset->setScale(osg::Vec3(scale, scale, scale));
}

void OsgBone::createLeafJoint()
{
}

void OsgBone::updateStickManRenderingState(bool renderStickMan)
{
	m_toggleStickManRendering = renderStickMan;
}