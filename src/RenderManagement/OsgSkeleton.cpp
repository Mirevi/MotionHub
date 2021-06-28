#include "OsgSkeleton.h"

//#include "OsgLine.h"
//#include "OsgBone.h"
#include "OsgBone.h"

//HWM: Look in Desktop/p../skel.PNG and skel2.PNG and DEVDOC

OsgSkeleton::OsgSkeleton(osg::ref_ptr<osg::Group> parentNode) : m_parentNode(parentNode)
{
	m_skeletonRootNode = new osg::Group();
	// create 21 joints
	osg::ref_ptr<osg::PositionAttitudeTransform> tempJoint; // = new osg::PositionAttitudeTransform();
	for (int indexJoint = 0; indexJoint < 21; indexJoint++)
	{

		m_joints.push_back(new osg::PositionAttitudeTransform());
		m_skeletonRootNode->addChild(m_joints.at(indexJoint));	


			switch (indexJoint)
			{
			case 0:
				tempJoint = m_joints.at(indexJoint);
				break;
			case 1:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 2:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 3:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 4:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 5:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 6:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 7: //Leaf
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));


				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), 1.0f));
				break;
			case 8:
				m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 9:
				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), m_joints.at(indexJoint)));
				m_joints.at(indexJoint)->addChild(m_bones.at(indexJoint));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 10:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 11://Leaf
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);

				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), 1.0f));
				break;
				break;
			case 12:
				m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 13:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 14:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 15: //Leaf
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));
				tempJoint = m_joints.at(indexJoint);


				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), 1.0f));
				break;
			case 16:
				m_bones.push_back(new OsgBone(m_joints.at(0), m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 17:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				
				break;
			case 18:
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint), osg::Quat(osg::DegreesToRadians(90.0), osg::Z_AXIS)));
				tempJoint = m_joints.at(indexJoint);
				break;
			case 19: //Leaf
				m_bones.push_back(new OsgBone(tempJoint, m_joints.at(indexJoint)));


				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), 1.0f));
				break;
			case 20: //Leaf
				m_bones.push_back(new OsgBone(m_joints.at(3), m_joints.at(indexJoint)));


				m_bones.push_back(new OsgBone(m_joints.at(indexJoint), 1.0f));
				m_joints.at(indexJoint)->addChild(m_bones.at(indexJoint - 1));
				break;
			dafault:
				break;
			}



		//OsgBone* bone = new OsgBone(, );
		//m_bones.push_back(bone);
		//m_joints.at(indexJoint)->addChild(m_bones.at(indexJoint));

	}
	//m_line = new OsgLine(m_skeletonRootNode, false);
	//m_skeletonRootNode->addChild(m_line);

	m_parentNode->addChild(m_skeletonRootNode);
}


OsgSkeleton::~OsgSkeleton()
{
	//m_externalRootNode->removeChild(m_skeletonRootNode);
	//delte m_joints
	//delte m_bones
	//Avoid ref ptrs? Better readable for novices!

}


void OsgSkeleton::update(Skeleton skeleton)
{
	std::cout << "update() von OsgSkeleton" << std::endl;
	//m_line->clear();

	int indexJoint = 0;

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

		m_joints.at(indexJoint)->setScale(osg::Vec3f(0.1f, 0.1f, 0.1f));

		switch (itJoint->first) {
		case Joint::HIPS:
		{
			// HWM
			//m_bones			
			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));

			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}

		break;
		case Joint::SPINE:
		{
			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));

			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}

		break;
		case Joint::CHEST:
		{
			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(-90.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));

			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}

		break;
		case Joint::NECK:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::SHOULDER_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));


		}
		break;
		case Joint::ARM_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));


		}
		break;
		case Joint::FOREARM_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::HAND_L:
		{

		}

		break;
		case Joint::SHOULDER_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));
		}
		break;
		case Joint::ARM_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));
		}
		break;
		case Joint::FOREARM_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));

			m_joints.at(indexJoint)->setAttitude(osg::Quat(osg::DegreesToRadians(180.0), osg::Z_AXIS) * osg::Quat(itJoint->second.getJointRotation().x(),
				itJoint->second.getJointRotation().y(),
				itJoint->second.getJointRotation().z(),
				itJoint->second.getJointRotation().w()));
		}
		break;
		case Joint::HAND_R:
		{

		}
		break;
		case Joint::UPLEG_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::LEG_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::FOOT_L:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::TOE_L:

			break;
		case Joint::UPLEG_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::LEG_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::FOOT_R:
		{
			osg::Vec3f boneVector = osg::Vec3(itJoint->second.getJointPosition().x(),
				itJoint->second.getJointPosition().y(),
				itJoint->second.getJointPosition().z())
				-
				osg::Vec3((std::next(itJoint))->second.getJointPosition().x(),
					std::next(itJoint)->second.getJointPosition().y(),
					std::next(itJoint)->second.getJointPosition().z());
			float boneLength = boneVector.length();

			m_joints.at(indexJoint)->setScale(osg::Vec3(boneLength, boneLength, boneLength));
		}
		break;
		case Joint::TOE_R:
		{

		}
		break;
		case Joint::HEAD:
		{

		}
		break;

		default:
			break;
		}

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

//void MmhSkeleton::setMesh(osg::Node* node)
//{
//	//push back X bones
//	//m_bones.push_back(osgDB::readNodeFile("./data/mesh_models/bone.obj"));
//
//}
