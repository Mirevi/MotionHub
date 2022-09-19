#include "OsgBone.h"
#include <osgDB/ReadFile>
#include "OsgLine.h"


//CTOR for start and end joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint,
	osg::ref_ptr<osg::PositionAttitudeTransform> endJoint,
	osg::ref_ptr<osg::Group> stickManGroup,
	bool isStickManRenderingActive,
	osg::Quat rotationOffset)
	: m_startJoint(startJoint), m_endJoint(endJoint), m_isStickManRenderingActive(isStickManRenderingActive)
{

	initialize(stickManGroup, rotationOffset);

}


//CTOR for leaf joint with only one joint
OsgBone::OsgBone(osg::ref_ptr<osg::PositionAttitudeTransform> startJoint,
	osg::ref_ptr<osg::Group> stickManGroup,
	bool isStickManRenderingActive,
	osg::Quat rotationOffset)
	: m_startJoint(startJoint), m_isStickManRenderingActive(isStickManRenderingActive)
{

	// m_endJoint won`t be initialzed and is the hint for being an end joint - a "end joint" without any further connections  
	initialize(stickManGroup, rotationOffset);

}

void OsgBone::initialize(osg::ref_ptr<osg::Group> stickManGroup, osg::Quat rotationOffset)
{

	m_boneNode = osgDB::readNodeFile("./data/mesh_models/bone.obj");
	m_startJointOffset = new osg::PositionAttitudeTransform();
	m_startJointOffset->addChild(m_boneNode);
	m_startJointOffset->setAttitude(rotationOffset);
	m_startJoint->addChild(m_startJointOffset);
	m_line = new OsgLine(stickManGroup, true);
	m_isGlLookAtSolidBoneRotationActivated = false;

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
	
	if (m_endJoint) //Check if a leaf bone, then do not use the endJoint, because it is not initialized. Leaf bone is an "end bone" without further joint connections
	{
		//Calculate scale.Scale is the length (magnitude) of the vector between start and end joint. 
		osg::Vec3f boneVector = m_startJoint->getPosition() - m_endJoint->getPosition();
		float boneLength = boneVector.length();
		m_startJointOffset->setScale(osg::Vec3(boneLength, boneLength, boneLength));

		if (m_isGlLookAtSolidBoneRotationActivated)
		{
			osg::Matrix matrix;
			matrix.makeLookAt(m_startJoint->getPosition(), m_endJoint->getPosition(), osg::Y_AXIS);
			m_startJointOffset->setAttitude(matrix.getRotate());
			m_startJointOffset->setPosition(m_startJoint->getPosition());
			//m_startJointOffset->matr
			//osg::look boneVector
		}

		// Perform line rendering for stick man rendering
		if (m_isStickManRenderingActive)
		{
			m_line->clear();
			// create a  line
			m_line->draw(m_startJoint->getPosition(), m_endJoint->getPosition(),
				osg::Vec4f(0.95f, 0.95f, 0.95f, 1.0f),
				osg::Vec4f(0.61f, 0.33f, 0.05f, 1.0f));
			// to reset the line count to 0, use clear(). After this, Line is empty and new lines can be added
			m_line->redraw();
		}
		else
		{
			m_line->clear();
			m_line->redraw();
		}
	}
	else //It`s a leaf bone
	{
		m_startJointOffset->setScale(osg::Vec3(0.1f, 0.1f, 0.1f));

		//TODO1:if (m_isGlLookAtSolidBoneRotationActivated)

		if (m_isStickManRenderingActive)
		{
			m_line->clear();
			// create a  line
			m_line->draw(m_startJoint->getPosition(),
				m_startJoint->getPosition() + m_startJointOffset->getAttitude() * m_startJoint->getAttitude() * osg::Vec3f(-0.1f, 0.0f, 0.0f),
				osg::Vec4f(0.95f, 0.95f, 0.95f, 1.0f),
				osg::Vec4f(0.7f, 0.38f, 0.08f, 1.0f));
			// to reset the line count to 0, use clear(). After this, Line is empty and new lines can be added
			m_line->redraw();
		}
		else
		{
			m_line->clear();
			m_line->redraw();
		}
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
	m_isStickManRenderingActive = renderStickMan;
}