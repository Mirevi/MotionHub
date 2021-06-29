#include "OsgSkeleton.h"

//#include "OsgLine.h"
//#include "OsgBone.h"
#include "OsgBone.h"
#include <osg/Program>

//HWM: Look in Desktop/p../skel.PNG and skel2.PNG and DEVDOC

OsgSkeleton::OsgSkeleton(osg::ref_ptr<osg::Group> parentNode) : m_parentNode(parentNode)
{
	m_skeletonRootNode = new osg::Group();
	//m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Program> program = new osg::Program;
	osg::ref_ptr<osg::Shader> vshader = osg::Shader::readShaderFile(osg::Shader::VERTEX, "data/shader/FLAT_SHADING.vert");
	program->addShader(vshader.get());
	osg::ref_ptr<osg::Shader> gshader = osg::Shader::readShaderFile(osg::Shader::GEOMETRY, "data/shader/FLAT_SHADING.geom");
	program->addShader(gshader.get());
	osg::ref_ptr<osg::Shader> fshader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, "data/shader/FLAT_SHADING.frag");
	program->addShader(fshader.get());
	m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	m_skeletonRootNode->getOrCreateStateSet()->setAttributeAndModes(program.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	program->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
	program->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
	program->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);


	m_skeletonRootNode->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::ON);


	// create 21 joints
	osg::ref_ptr<osg::PositionAttitudeTransform> tempJoint; // = new osg::PositionAttitudeTransform();
	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
	{

		m_joints.push_back(new osg::PositionAttitudeTransform());
		m_skeletonRootNode->addChild(m_joints.at(indexJoint));

		//OsgBone* bone = new OsgBone(, );
		//m_bones.push_back(bone);
		//m_joints.at(indexJoint)->addChild(m_bones.at(indexJoint));

	}
	//m_line = new OsgLine(m_skeletonRootNode, false);
	//m_skeletonRootNode->addChild(m_line);
	m_parentNode->addChild(m_skeletonRootNode);

	//Build the skeleton
	//HIPS to SPINE bone
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(1), osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//SPINE to CHEST bone
	m_bones.push_back(new OsgBone(m_joints.at(1), m_joints.at(2), osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//CHEST to NECK bone
	m_bones.push_back(new OsgBone(m_joints.at(2), m_joints.at(3), osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//NECK to SHOULDER_L bone
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(4), osg::Quat(osg::DegreesToRadians(45.0), osg::Z_AXIS)));
	//SHOULDER_L to ARM_L bone
	m_bones.push_back(new OsgBone(m_joints.at(4), m_joints.at(5)));
	//ARM_L to FOREARM_L bone
	m_bones.push_back(new OsgBone(m_joints.at(5), m_joints.at(6)));
	//FOREARM_L to HAND_L bone
	m_bones.push_back(new OsgBone(m_joints.at(6), m_joints.at(7)));
	//HAND_L ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(7), 1.0f));
	//NECK to SHOULDER_R bone
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(8), osg::Quat(osg::DegreesToRadians(135.0), osg::Z_AXIS))); // 
	//SHOULDER_R to ARM_R bone
	m_bones.push_back(new OsgBone(m_joints.at(8), m_joints.at(9), osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//ARM_R to FOREARM_R bone
	m_bones.push_back(new OsgBone(m_joints.at(9), m_joints.at(10), osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//FOREARM_R to HAND_R bone
	m_bones.push_back(new OsgBone(m_joints.at(10), m_joints.at(11), osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//HAND_R ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(11), 1.0f, osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//HIPS to UPLEG_L
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(12))); //-180 // , osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)
	//UPLEG_L to LEG_L 
	m_bones.push_back(new OsgBone(m_joints.at(12), m_joints.at(13), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//LEG_L to FOOT_L
	m_bones.push_back(new OsgBone(m_joints.at(13), m_joints.at(14), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//FOOT_L to TOE_L - Rotation to TOE_L was visually adjusted in the render view
	m_bones.push_back(new OsgBone(m_joints.at(14), m_joints.at(15), osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS) * osg::Quat(osg::DegreesToRadians(-37.0), osg::X_AXIS)));
	//TOE_L ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(15), 1.0f, osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS)));
	//HIPS to UPLEG_R
	m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(16), osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS)));
	//UPLEG_R to LEG_R
	m_bones.push_back(new OsgBone(m_joints.at(16), m_joints.at(17), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//LEG_R to Foot_R
	m_bones.push_back(new OsgBone(m_joints.at(17), m_joints.at(18), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
	//FOOT_R to TOE_R - Rotation to TOE_R was visually adjusted in the render view
	m_bones.push_back(new OsgBone(m_joints.at(18), m_joints.at(19), osg::Quat(osg::DegreesToRadians(-90.0), osg::Y_AXIS) * osg::Quat(osg::DegreesToRadians(-37.0), osg::X_AXIS)));
	//TOE_R ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(19), 1.0f, osg::Quat(osg::DegreesToRadians(90.0), osg::Y_AXIS)));
	//NECK to HEAD
	m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(20), osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
	//HEAD ### leaf bone ###
	m_bones.push_back(new OsgBone(m_joints.at(20), 1.0f, osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS)));
}


OsgSkeleton::~OsgSkeleton()
{
	//m_externalRootNode->removeChild(m_skeletonRootNode);
	//delte m_joints
	//delte m_bones
	//Avoid ref ptrs? Better readable for novices!
	//m_skeletonRootNode->removeChildren(0, m_skeletonRootNode->getNumChildren());


	std::cout << "DTOR OsgSkeleton" << std::endl;

}


void OsgSkeleton::update(Skeleton skeleton)
{
	std::cout << "update() von OsgSkeleton" << std::endl;
	//m_line->clear();

	int indexJoint = 0;

	//Update alle joints mit Pos und Att
	//Update alle bones mit Scale -> Scale muss auf den PosAtt des Bones angewendet werden


	// update each joint
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


		indexJoint++;
	}

	// update each bone
	for (int i = 0; i < m_bones.size(); i++)
	{
		m_bones.at(i)->updateScale();
	}











	indexJoint = 0;


	//// update confidence
	for (auto itJoint = skeleton.m_joints.begin(); itJoint != skeleton.m_joints.end(); itJoint++)
	{
	//	m_joints.at(indexJoint)->setPosition(osg::Vec3(
	//		itJoint->second.getJointPosition().x(),
	//		itJoint->second.getJointPosition().y(),
	//		itJoint->second.getJointPosition().z()));

	//	m_joints.at(indexJoint)->setAttitude(osg::Quat(itJoint->second.getJointRotation().x(),
	//		itJoint->second.getJointRotation().y(),
	//		itJoint->second.getJointRotation().z(),
	//		itJoint->second.getJointRotation().w()));

	//	m_joints.at(indexJoint)->setScale(osg::Vec3f(0.1f, 0.1f, 0.1f));



		//switch (itJoint->first) {
		//case Joint::HIPS:
		//{
		//	// HWM
		//	//m_bones			
		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));

		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}

		//break;
		//case Joint::SPINE:
		//{
		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));

		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}

		//break;
		//case Joint::CHEST:
		//{
		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));

		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}

		//break;
		//case Joint::NECK:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::SHOULDER_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));


		//}
		//break;
		//case Joint::ARM_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));


		//}
		//break;
		//case Joint::FOREARM_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::HAND_L:
		//{

		//}

		//break;
		//case Joint::SHOULDER_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));
		//}
		//break;
		//case Joint::ARM_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));
		//}
		//break;
		//case Joint::FOREARM_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

		//	m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
		//		itJoint->second.getJointRotation().y(),
		//		itJoint->second.getJointRotation().z(),
		//		itJoint->second.getJointRotation().w()));
		//}
		//break;
		//case Joint::HAND_R:
		//{

		//}
		//break;
		//case Joint::UPLEG_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::LEG_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::FOOT_L:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::TOE_L:

		//	break;
		//case Joint::UPLEG_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::LEG_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::FOOT_R:
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z())
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();

		//	m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		//}
		//break;
		//case Joint::TOE_R:
		//{

		//}
		//break;
		//case Joint::HEAD:
		//{

		//}
		//break;

		//default:
		//	break;
		//}













		//if (indexJoint == 16)
		//{
		//	osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z()) 
		//		-
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//				std::next(itJoint)->second.getJointPosition().z());
		//	float boneLength = boneVector.length();
		//	
		//	m_line->draw(osg::Vec3(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z()),
		//		osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
		//			std::next(itJoint)->second.getJointPosition().y(),
		//			std::next(itJoint)->second.getJointPosition().z()),
		//		osg::Vec4f(1.0, 0.0, 0.0, 1.0),
		//		osg::Vec4f(0.0, 1.0, 0.0, 1.0));

			//osg::Matrix test;
			//test.makeLookAt(osg::Vec3(itJoint->second.getJointPosition().x(),
			//	itJoint->second.getJointPosition().y(),
			//	itJoint->second.getJointPosition().z()),
			//	osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
			//		std::next(itJoint)->second.getJointPosition().y(),
			//		std::next(itJoint)->second.getJointPosition().z()), osg::X_AXIS);
			//m_joints.at(indexJoint)->setAttitude(test.getRotate());


		//}
		//m_line->redraw();








		//Console::log("GlWidget::updateSkeletonMeshTransform(): pool size = " + toString(m_skeletonMeshPool.size()));

		// get current skeleton
		//auto currMeshTracker = m_skeletonMeshPool.find((*itTracker)->getProperties()->id);

		//juj: Was ist hiermit???
		//if (currMeshTracker->second.size() == 0)
		//{
		//	return;
		//}

		// get current joint
		//Cube* currJoint = currMeshTracker->second.at(indexSkeleton).m_joints[indexJoint];

		// set joint position and rotation
		//currJoint->setPosition(itJoint->second.getJointPosition());
		//currJoint->setRotation(itJoint->second.getJointRotation());

		//osg::Matrix transformMatrix;
		//transformMatrix = osg::Matrix::rotate(osg::Quat(itJoint->second.getJointRotation().x(),
		//	itJoint->second.getJointRotation().y(),
		//	itJoint->second.getJointRotation().z(),
		//	itJoint->second.getJointRotation().w()))
		//	* osg::Matrix::translate(osg::Vec3f(itJoint->second.getJointPosition().x(),
		//		itJoint->second.getJointPosition().y(),
		//		itJoint->second.getJointPosition().z()));
		//m_sphereTransforms.at(indexJoint)->setMatrix(transformMatrix);


	//	m_axesCrosses.at(indexJoint)->setAttitude(osg::Quat(itJoint->second.getJointRotation().x(),
	//		itJoint->second.getJointRotation().y(),
	//		itJoint->second.getJointRotation().z(),
	//		itJoint->second.getJointRotation().w()));
	//m_axesCrosses.at(indexJoint)->setPosition(osg::Vec3f(itJoint->second.getJointPosition().x(),
	//	itJoint->second.getJointPosition().y(),
	//	itJoint->second.getJointPosition().z()));




	// #### START debug draws: AxesCross and Lines 2/2 ####
	//// set attitude sets a rotation. Can also be used for the line
	//m_axesCrossTest->setAttitude(osg::Quat(itJoint->second.getJointRotation().x(),
	//	itJoint->second.getJointRotation().y(),
	//	itJoint->second.getJointRotation().z(),
	//	itJoint->second.getJointRotation().w()));
	//// set position
	//m_axesCrossTest->setPosition(osg::Vec3f(itJoint->second.getJointPosition().x(),
	//	itJoint->second.getJointPosition().y(),
	//	itJoint->second.getJointPosition().z()));
	//// each time, the line has changed, redraw() must be called. Here are no changes in this loop, so it would be ok to only call redraw() once after creation
	////However, if changes will be made here in the loop, redraw(9 must be called. Otherwise, no changes are displayed
	//m_line->redraw();
	// ---- END debug draws ----



	// set joint confidence in the shader
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

//void MmhSkeleton::setMesh(osg::Node* node)
//{
//	//push back X bones
//	//m_bones.push_back(osgDB::readNodeFile("./data/mesh_models/bone.obj"));
//
//}
