#include "OsgSkeleton.h"
#include "OsgBone.h"
#include "OsgAxesCross.h"
#include <osg/Program>

OsgSkeleton::OsgSkeleton(osg::ref_ptr<osg::Group> parentNode) : m_parentNode(parentNode)
{
	m_skeletonRootNode = new osg::Group();
	m_boneGroup = new osg::Group();
	m_stickManGroup = new osg::Group();
	m_axesCrossGroup = new osg::Group();
	m_skeletonRootNode->addChild(m_boneGroup);
	m_skeletonRootNode->addChild(m_stickManGroup);
	m_skeletonRootNode->addChild(m_axesCrossGroup);

	m_toggleStickManRendering = true;

	//m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Program> program = new osg::Program;
	osg::ref_ptr<osg::Shader> vshader = osg::Shader::readShaderFile(osg::Shader::VERTEX, "data/shader/FLAT_SHADING.vert");
	program->addShader(vshader.get());
	osg::ref_ptr<osg::Shader> gshader = osg::Shader::readShaderFile(osg::Shader::GEOMETRY, "data/shader/FLAT_SHADING.geom");
	program->addShader(gshader.get());
	osg::ref_ptr<osg::Shader> fshader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, "data/shader/FLAT_SHADING.frag");
	program->addShader(fshader.get());
	m_boneGroup->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	m_boneGroup->getOrCreateStateSet()->setAttributeAndModes(program.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);


	m_boneGroup->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);


	// create 21 joints
	osg::ref_ptr<osg::PositionAttitudeTransform> tempJoint; // = new osg::PositionAttitudeTransform();
	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
	{
		m_joints.push_back(new osg::PositionAttitudeTransform());
		m_boneGroup->addChild(m_joints.at(indexJoint));
	}
	m_parentNode->addChild(m_skeletonRootNode);

	//Build the skeleton
	//HIPS to SPINE bone
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(1), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//SPINE to CHEST bone
	m_bones.push_back(new OsgBone(m_joints.at(1), m_joints.at(2), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//CHEST to NECK bone
	m_bones.push_back(new OsgBone(m_joints.at(2), m_joints.at(3), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//NECK to SHOULDER_L bone
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(4), m_stickManGroup, osg::Quat(osg::DegreesToRadians(45.0), osg::Z_AXIS)));
	//SHOULDER_L to ARM_L bone
	m_bones.push_back(new OsgBone(m_joints.at(4), m_joints.at(5), m_stickManGroup));
	//ARM_L to FOREARM_L bone
	m_bones.push_back(new OsgBone(m_joints.at(5), m_joints.at(6), m_stickManGroup));
	//FOREARM_L to HAND_L bone
	m_bones.push_back(new OsgBone(m_joints.at(6), m_joints.at(7), m_stickManGroup));
	//HAND_L ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(7), 1.0f, m_stickManGroup));
	//NECK to SHOULDER_R bone
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(8), m_stickManGroup, osg::Quat(osg::DegreesToRadians(135.0), osg::Z_AXIS))); // 
	//SHOULDER_R to ARM_R bone
	m_bones.push_back(new OsgBone(m_joints.at(8), m_joints.at(9), m_stickManGroup, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//ARM_R to FOREARM_R bone
	m_bones.push_back(new OsgBone(m_joints.at(9), m_joints.at(10), m_stickManGroup, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//FOREARM_R to HAND_R bone
	m_bones.push_back(new OsgBone(m_joints.at(10), m_joints.at(11), m_stickManGroup, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//HAND_R ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(11), 1.0f, m_stickManGroup, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//HIPS to UPLEG_L
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(12), m_stickManGroup)); //-180 // , osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)
	//UPLEG_L to LEG_L 
	m_bones.push_back(new OsgBone(m_joints.at(12), m_joints.at(13), m_stickManGroup, osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//LEG_L to FOOT_L
	m_bones.push_back(new OsgBone(m_joints.at(13), m_joints.at(14), m_stickManGroup, osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//FOOT_L to TOE_L - Rotation to TOE_L was visually adjusted in the render view
	m_bones.push_back(new OsgBone(m_joints.at(14), m_joints.at(15), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS) * osg::Quat(osg::DegreesToRadians(-37.0), osg::X_AXIS)));
	//TOE_L ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(15), 1.0f, m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS)));
	//HIPS to UPLEG_R
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(16), m_stickManGroup, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//UPLEG_R to LEG_R
	m_bones.push_back(new OsgBone(m_joints.at(16), m_joints.at(17), m_stickManGroup, osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//LEG_R to Foot_R
	m_bones.push_back(new OsgBone(m_joints.at(17), m_joints.at(18), m_stickManGroup, osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//FOOT_R to TOE_R - Rotation to TOE_R was visually adjusted in the render view
	m_bones.push_back(new OsgBone(m_joints.at(18), m_joints.at(19), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS) * osg::Quat(osg::DegreesToRadians(-37.0), osg::X_AXIS)));
	//TOE_R ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(19), 1.0f, m_stickManGroup, osg::Quat(osg::DegreesToRadians(90.0), osg::Y_AXIS)));
	//NECK to HEAD
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(20), m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//HEAD ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(20), 1.0f, m_stickManGroup, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));


	//RGB axes for each joint
	m_toggleJointAxes = false;
	//m_axesCrossGroup->setNodeMask(0x0t); //Hide from scenegraph
	//setup RGB joint axes
	for (int i = 0; i <= 20; i++)
	{
		m_axesCrosses.push_back(new OsgAxesCross(m_axesCrossGroup));

	}

}


OsgSkeleton::~OsgSkeleton()
{
	//TODO1: Write DTOR
	//m_externalRootNode->removeChild(m_skeletonRootNode);
	//delte m_joints
	//delte m_bones
	//Avoid ref ptrs? Better readable for novices!
	//m_skeletonRootNode->removeChildren(0, m_skeletonRootNode->getNumChildren());

}


void OsgSkeleton::update(Skeleton skeleton)
{

	int indexJoint = 0;

	// update each joint and RGB joint axes
	for (auto itJoint = skeleton.m_joints.begin(); itJoint != skeleton.m_joints.end(); itJoint++)
	{
		m_joints.at(indexJoint)->setPosition(osg::Vec3(
			itJoint->second.getJointPosition().x(),
			itJoint->second.getJointPosition().y(),
			itJoint->second.getJointPosition().z()));

		m_joints.at(indexJoint)->setAttitude(osg::Quat(itJoint->second.getJointRotation().x(),
			itJoint->second.getJointRotation().y(),
			itJoint->second.getJointRotation().z(),
			itJoint->second.getJointRotation().w()));

		m_axesCrosses.at(indexJoint)->setAttitude(osg::Quat(itJoint->second.getJointRotation().x(),
			itJoint->second.getJointRotation().y(),
			itJoint->second.getJointRotation().z(),
			itJoint->second.getJointRotation().w()));
		m_axesCrosses.at(indexJoint)->setPosition(osg::Vec3f(itJoint->second.getJointPosition().x(),
			itJoint->second.getJointPosition().y(),
			itJoint->second.getJointPosition().z()));


		indexJoint++;
	}

	// update each bone
	for (int i = 0; i < m_bones.size(); i++)
	{
		m_bones.at(i)->update();
		m_bones.at(i)->updateStickManRenderingState(m_toggleStickManRendering);
	}

	indexJoint = 0;

	//// update confidence
	for (auto itJoint = skeleton.m_joints.begin(); itJoint != skeleton.m_joints.end(); itJoint++)
	{
		// TODO2: set joint confidence in the shader
		switch (itJoint->second.getJointConfidence())
		{

		case Joint::HIGH:
			//currJoint->setDiffuseColor(m_colorGreen);
			//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorGreen.m_xyz.x, m_colorGreen.m_xyz.y, m_colorGreen.m_xyz.z, 1.0f));
			break;

		case Joint::MEDIUM:
			//currJoint->setDiffuseColor(m_colorYellow);
			//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorYellow.m_xyz.x, m_colorYellow.m_xyz.y, m_colorYellow.m_xyz.z, 1.0f));
			break;

		case Joint::LOW:
			//currJoint->setDiffuseColor(m_colorRed);
			//m_spheres.at(indexJoint)->setColor(osg::Vec4f(m_colorRed.m_xyz.x, m_colorRed.m_xyz.y, m_colorRed.m_xyz.z, 1.0f));
			break;

		case Joint::NONE:
			//currJoint->setDiffuseColor(Vector3::one());
			//m_spheres.at(indexJoint)->setColor(osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
			break;

		default:
			break;

		}

		indexJoint++;

	}

}


void OsgSkeleton::removeAndDelete()
{
	m_skeletonRootNode->removeChildren(0, m_skeletonRootNode->getNumChildren());
	//std::cout << "m_skeletonRootNode->referenceCount"<< m_skeletonRootNode->referenceCount() << std::endl;
	//std::cout << "m_bones" << m_skeletonRootNode->referenceCount() << std::endl;
	//std::cout << "remove and delete called from OsgSkeleton" << std::endl;
	m_skeletonRootNode = NULL; // Should be removed by OSG

	for (int i = 0; i < m_bones.size(); i++)
	{
		delete m_bones.at(i);
	}
}


void OsgSkeleton::toggleJointAxes(bool menuValue)
{

	m_toggleJointAxes = menuValue;
	if (m_toggleJointAxes)
	{
		m_axesCrossGroup->setNodeMask(0x1); //Show to scenegraph
	}
	else
	{
		m_axesCrossGroup->setNodeMask(0x0); //Hide from scenegraph
	}

}


void OsgSkeleton::toggleStickManRendering(bool menuValue){

	std::cout << "toggleStickManRendering value is " << menuValue << std::endl;
	m_toggleStickManRendering = menuValue;
	if (m_toggleStickManRendering)
	{
		m_stickManGroup->setNodeMask(0x1); //Show to scenegraph
	}
	else
	{
		m_stickManGroup->setNodeMask(0x0); //Hide from scenegraph
	}

}

void OsgSkeleton::toggleSolidBoneRendering(bool menuValue){

	m_toggleSolidBoneRendering = menuValue;
	if (m_toggleSolidBoneRendering)
	{
		m_boneGroup->setNodeMask(0x1); //Show to scenegraph
	}
	else
	{
		m_boneGroup->setNodeMask(0x0); //Hide from scenegraph
	}

}

