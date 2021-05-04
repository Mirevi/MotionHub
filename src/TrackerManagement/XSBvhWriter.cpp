#include "XSBvhWriter.h"



// default constructor
XSBvhWriter::XSBvhWriter(enum class SkeletonType skeletonType, UdpServer* udpServer, int avatarId)
{
	m_UdpServer = udpServer;
	m_avatarId = avatarId;
	// init BVH parameters
	setupSkeleton(skeletonType);
	setupBvhParameters();

	m_recordingThread = new std::thread(&XSBvhWriter::writeMotionDataInBvhStream, this);

}

XSBvhWriter::~XSBvhWriter()
{



}


void XSBvhWriter::setupSkeleton(SkeletonType skeletonType) {
	m_joints = new std::vector<BvhJoint>();


	switch (skeletonType)
	{

	case SkeletonType::Xsens:
		// Xsens Skeleton with 23 joints
		// xsens joint id, name, parentId, bool endeffector
		m_joints->push_back(BvhJoint({ 0, "Hips", -1, false }));
		m_joints->push_back(BvhJoint({ 1, "Chest", XsJointHierarchy::Hips, false }));
		m_joints->push_back(BvhJoint({ 2, "Chest2", XsJointHierarchy::Chest, false }));
		m_joints->push_back(BvhJoint({ 3, "Chest3", XsJointHierarchy::Chest2, false }));
		m_joints->push_back(BvhJoint({ 4, "Chest4", XsJointHierarchy::Chest3, false }));
		m_joints->push_back(BvhJoint({ 5, "Neck", XsJointHierarchy::Chest4, false }));
		m_joints->push_back(BvhJoint({ 6, "Head", XsJointHierarchy::Neck, true }));
		m_joints->push_back(BvhJoint({ 7, "RightCollar", XsJointHierarchy::Chest4, false }));
		m_joints->push_back(BvhJoint({ 8, "RightShoulder", XsJointHierarchy::RightCollar, false }));
		m_joints->push_back(BvhJoint({ 9, "RightElbow", XsJointHierarchy::RightShoulder, false }));
		m_joints->push_back(BvhJoint({ 10, "RightWrist", XsJointHierarchy::RightElbow, true }));
		m_joints->push_back(BvhJoint({ 11, "LeftCollar", XsJointHierarchy::Chest4, false }));
		m_joints->push_back(BvhJoint({ 12, "LeftShoulder", XsJointHierarchy::LeftCollar, false }));
		m_joints->push_back(BvhJoint({ 13, "LeftElbow", XsJointHierarchy::LeftShoulder, false }));
		m_joints->push_back(BvhJoint({ 14, "LeftWrist", XsJointHierarchy::LeftElbow, true }));
		m_joints->push_back(BvhJoint({ 15, "RightHip", XsJointHierarchy::Hips, false }));
		m_joints->push_back(BvhJoint({ 16, "RightKnee", XsJointHierarchy::RightHip, false }));
		m_joints->push_back(BvhJoint({ 17, "RightAnkle", XsJointHierarchy::RightKnee, false }));
		m_joints->push_back(BvhJoint({ 18, "RightToe", XsJointHierarchy::RightAnkle , true }));
		m_joints->push_back(BvhJoint({ 19, "LeftHip", XsJointHierarchy::Hips, false }));
		m_joints->push_back(BvhJoint({ 20, "LeftKnee", XsJointHierarchy::LeftHip , false }));
		m_joints->push_back(BvhJoint({ 21, "LeftAnkle", XsJointHierarchy::LeftKnee , false }));
		m_joints->push_back(BvhJoint({ 22, "LeftToe", XsJointHierarchy::LeftAnkle, true }));

		break;

	case SkeletonType::MMH:
		// MMH Skeleton with 21 joints 
		// xsens joint id, name, parentId, bool endeffector
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::HIPS, "HIPS", -1, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::SPINE, "SPINE", MMHJointHierarchy::HIPS, false }));
		// Chest4 important vor BVH
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::CHEST, "CHEST", MMHJointHierarchy::SPINE, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::NECK, "NECK", MMHJointHierarchy::CHEST, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::HEAD, "HEAD", MMHJointHierarchy::NECK, true }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::SHOULDER_L, "SHOULDER_L", MMHJointHierarchy::CHEST, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::ARM_L, "ARM_L", MMHJointHierarchy::SHOULDER_L, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::FOREARM_L, "FOREARM_L", MMHJointHierarchy::ARM_L, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::HAND_L, "HAND_L", MMHJointHierarchy::FOREARM_L, true }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::SHOULDER_R, "SHOULDER_R", MMHJointHierarchy::CHEST, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::ARM_R, "ARM_R", MMHJointHierarchy::SHOULDER_R, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::FOREARM_R, "FOREARM_R", MMHJointHierarchy::ARM_R, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::HAND_R, "HAND_R", MMHJointHierarchy::FOREARM_R, true }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::UPLEG_L, "UPLEG_L", MMHJointHierarchy::HIPS, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::LEG_L, "LEG_L", MMHJointHierarchy::UPLEG_L, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::FOOT_L, "FOOT_L", MMHJointHierarchy::LEG_L, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::TOE_L, "TOE_L", MMHJointHierarchy::FOOT_L, true }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::UPLEG_R, "UPLEG_R", MMHJointHierarchy::HIPS, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::LEG_R, "LEG_R", MMHJointHierarchy::UPLEG_R , false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::FOOT_R, "FOOT_R", MMHJointHierarchy::LEG_R, false }));
		m_joints->push_back(BvhJoint({ MMHJointHierarchy::TOE_R, "TOE_R", MMHJointHierarchy::FOOT_R , true }));


		break;

	default:
		break;
	}



}

void XSBvhWriter::setupBvhParameters() {
	// TIME STAMP
	time_t t = time(0);   // get time now
	struct tm* now = localtime(&t);
	char timestamp[80];
	localtime_s(now, &t);
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", now);

	//TIME STAMP !!!! // AVATAR ID
	bvhFileName = "MHH_bvh_export_" + std::to_string(m_avatarId) + "_" + timestamp;
	bvhExportPath = "C:/Users/Rainer/Desktop/ma git zwischenablage/MMH bvhs/";
	bvhExportFilePath = bvhExportPath + bvhFileName + ".bvh";
	bvhExportFilePathTemp = bvhExportPath + "Temp_" + bvhFileName + ".bvh";

	// start temporary motion stream
	bvhMotionOutputStreamTemp.open(bvhExportFilePathTemp);
	assert(bvhStream.is_open() && "Failed to open the output file.");
}


void XSBvhWriter::writeFinalBvhFile() {
	// close motion stream, then open motion file
	bvhMotionOutputStreamTemp.close();
	bvhMotionInputStreamTemp.open(bvhExportFilePathTemp);
	// start final bvh stream
	finalBvhStream.open(bvhExportFilePath);

	// get scale information
	if (bvhScaleInformation) {
		bvhScaleInformation = new ScaleDatagram::BvhScaleInformation();

		while (bvhScaleInformation->endSites.empty()) {
			bvhScaleInformation = m_UdpServer->getScaleDatagram();
			XsTime::msleep(50);
		}
	}


	std::cout << "WRITING BVH FILE..." << std::endl;

	// get position data of each skeleton joint + end effector positions
	std::vector<ScaleDatagram::NullPoseDefinition>* scaleData = &bvhScaleInformation->tPose;
	std::vector<ScaleDatagram::PointDefinition>* endSitesData = &bvhScaleInformation->endSites;
	std::vector<Vector3d> allOffsets;
	std::vector<Vector3d> allEndSiteOffsets;
	std::map<int, int> endSiteOffsets;

	// extract endSiteOffsets and generate index map
	for (int k = 0; k < endSitesData->size(); k++) {

		if (!endSitesData->at(k).segmentName.compare("pTopOfHead")) {
			endSiteOffsets[XsJointHierarchy::Head] = k;
		}
		else if (!endSitesData->at(k).segmentName.compare("pLeftTopOfHand")) {
			endSiteOffsets[XsJointHierarchy::LeftWrist] = k;
		}
		else if (!endSitesData->at(k).segmentName.compare("pRightTopOfHand")) {
			endSiteOffsets[XsJointHierarchy::RightWrist] = k;
		}
		else if (!endSitesData->at(k).segmentName.compare("pLeftToe")) {
			endSiteOffsets[XsJointHierarchy::LeftToe] = k;
		}
		else if (!endSitesData->at(k).segmentName.compare("pRightToe")) {
			endSiteOffsets[XsJointHierarchy::RightToe] = k;
		}

		// scale up by 100
		allEndSiteOffsets.push_back(Vector3d(endSitesData->at(k).pos[0] * 100, endSitesData->at(k).pos[1] * 100, endSitesData->at(k).pos[2] * 100));


	}

	// extract offsets of each joint, scale up by 100
	for (int k = 0; k < scaleData->size(); k++) {

		allOffsets.push_back(Vector3d(scaleData->at(k).pos[0] * 100, scaleData->at(k).pos[1] * 100, scaleData->at(k).pos[2] * 100));

	}


	// function for indent creation
	auto indent_creation = [](int depth) -> std::string
	{
		std::string tabs = "";
		for (int i = 0; i < depth; ++i) { tabs += "\t"; }
		return tabs;
	};


	// amount of indents
	int depth = 0;

	// write BVH Header
	finalBvhStream << "HIERARCHY" << "\n";
	// write root joint
	finalBvhStream << "ROOT " << m_joints->at(0).name << "\n";
	finalBvhStream << "{\n";
	depth += 1;
	// root is zero
	finalBvhStream << indent_creation(depth) << "OFFSET 0.000000 0.000000 0.000000" << "\n";
	// in case root is not zero, though for xsens root is always zero
	//finalBvhStream << std::fixed << std::setprecision(6) << indent_creation(depth) << "OFFSET " << 
	//	allEndSiteOffsets[endSiteOFfsets[m_joints->at(0).id]].y() << " " << 
	//	allEndSiteOffsets[endSiteOFfsets[m_joints->at(0).id]].z() << " " << 
	//	allEndSiteOffsets[endSiteOFfsets[m_joints->at(0).id]].x()  << "\n";
	finalBvhStream << indent_creation(depth) << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation\n";

	// round to nearest float
	fesetround(FE_TONEAREST);

	// iterate through all joints except root joint
	for (int i = 1; i < m_joints->size(); i++) {
		// for each joint write JOINT + joint name with right amount of indents
		finalBvhStream << indent_creation(depth) << "JOINT " << m_joints->at(i).name << "\n";
		finalBvhStream << indent_creation(depth) << "{\n";
		depth += 1;

		//current id
		int id = m_joints->at(i).id;
		int parentId = m_joints->at(i).parentId;

		// bvh joint offsets are relative to their parents position
		finalBvhStream << std::fixed << std::setprecision(6) << indent_creation(depth) << "OFFSET "
			<< allOffsets[id].y() - allOffsets[parentId].y() << " "
			<< allOffsets[id].z() - allOffsets[parentId].z() << " "
			<< allOffsets[id].x() - allOffsets[parentId].x() << "\n";

		// write channel order - Xsens default is YXZ
		finalBvhStream << indent_creation(depth) << "CHANNELS 3 Yrotation Xrotation Zrotation\n";

		// for end effectors write endsites
		if (m_joints->at(i).endsite) {
			finalBvhStream << indent_creation(depth) << "End Site\n";
			finalBvhStream << indent_creation(depth) << "{\n";
			depth += 1;
			// write endsite offset
			finalBvhStream << std::fixed << std::setprecision(6) << indent_creation(depth) << "OFFSET " <<
				allEndSiteOffsets[endSiteOffsets[m_joints->at(i).id]].y() << " " <<
				allEndSiteOffsets[endSiteOffsets[m_joints->at(i).id]].z() << " " <<
				allEndSiteOffsets[endSiteOffsets[m_joints->at(i).id]].x() << "\n";

			// if last joint, close hierarchy by writing "}" for each indent
			if (i == m_joints->size() - 1) {
				while (depth > 0) {
					depth -= 1;
					finalBvhStream << indent_creation(depth) << "}\n";
				}
			}
			// if not last joint, remove indents until on same line as parent
			else {
				//get index of parent
				int parentIndex = 0;

				for (auto itJoint = m_joints->begin(); itJoint != m_joints->end(); itJoint++)
				{
					if (itJoint->id == m_joints->at(i + 1).parentId) {
						break;
					}
					parentIndex++;
				}

				while (depth > parentIndex + 1) {
					depth -= 1;
					finalBvhStream << indent_creation(depth) << "}\n";
				}
			}

		}

	}

	int frameNumber = bvhFrameNumber;

	// write motion parameters
	finalBvhStream << "MOTION" << "\n";
	finalBvhStream << "Frames: " << frameNumber << "\n";
	finalBvhStream << std::fixed << std::setprecision(6) << "Frame Time: " << bvhFrameTime << "\n";

	//copy and write motion part from temporary motion file
	finalBvhStream << bvhMotionInputStreamTemp.rdbuf();
	finalBvhStream.close();
	bvhMotionInputStreamTemp.close();

	// delete temporary motion file
	std::remove(bvhExportFilePathTemp.c_str());

	//std::string cmd = "notepad.exe " + bvhExportFilePath;
	//system(cmd.c_str());
	std::cout << "BVH File saved as " << bvhFileName << ".bvh" << std::endl;

	isWriting = false;
}



void XSBvhWriter::writeMotionDataInBvhStream() {


	// Timer values
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	ParserManager::QuaternionDataWithId* quaternianDataWithId;

	// set Frames per Second
	int framesPerSecond = 60;
	bvhFrameTime = 1.0 / (double)framesPerSecond;
	// 1 s = 1000 ms = 1000000 micros
	int sleepInMicroSeconds = (int)(1000000 * bvhFrameTime);
	int frameCounter = 1;

	while (isRecording) {
		m_bvhLock.lock();

		// get absolute quaternion data from UdpServer
		quaternianDataWithId = m_UdpServer->getQuaternionDatagram();
		// check if data is valid
		if (quaternianDataWithId != NULL && quaternianDataWithId->kinematics != NULL && quaternianDataWithId->avatarId == m_avatarId) {


			std::vector<QuaternionDatagram::Kinematics>* kinematics = quaternianDataWithId->kinematics;
			std::vector<Quaterniond> allQuaternions;
			Vector3d rootPosition = Vector3d(kinematics->at(0).position[0], kinematics->at(0).position[1], kinematics->at(0).position[2]);

			// extract all values from datagram; NOTE: Xsens Skeleton has 23 joints by default
			for (int j = 0; j < 23; j++) {

				float w = XsMath_deg2rad(kinematics->at(j).orientation[0]);
				float x = XsMath_deg2rad(kinematics->at(j).orientation[1]);
				float y = XsMath_deg2rad(kinematics->at(j).orientation[2]);
				float z = XsMath_deg2rad(kinematics->at(j).orientation[3]);

				allQuaternions.push_back(Quaterniond(w, x, y, z));

			}


			// round to nearest float
			fesetround(FE_TONEAREST);

			//scale up and adjuts axis
			double rootPosX = rootPosition.y() * 100;
			double rootPosY = rootPosition.z() * 100;
			double rootPosZ = rootPosition.x() * 100;

			// write position data
			bvhMotionOutputStreamTemp << std::fixed << std::setprecision(6) << rootPosX << " " << rootPosY << " " << rootPosZ << " ";

			// write rotation data for each joint of skeleton
			for (int i = 0; i < m_joints->size(); i++) {

				BvhJoint* xsj = &m_joints->at(i);
				Quaterniond r;

				// for root joint use absolute rotation
				if (xsj->parentId == -1) {
					r = allQuaternions[xsj->id];
				}
				// for remaining joints calculate rotation relative to parent
				else {
					r = allQuaternions[xsj->parentId].inverse() * allQuaternions[xsj->id];
				}

				// convert quaternion to euler
				XsQuaternion xsQuat = XsQuaternion(r.w(), r.x(), r.y(), r.z());
				XsEuler euler;
				euler.fromQuaternion(xsQuat);

				// write rotation data
				bvhMotionOutputStreamTemp << std::fixed << std::setprecision(6) << euler.z() << " " << euler.y() << " " << euler.x() << " ";

			}

			// move to next line
			bvhMotionOutputStreamTemp << "\n";
			bvhFrameNumber++;

			// Wait for fps time
			while (true) {
				QueryPerformanceCounter(&EndingTime);
				ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
				ElapsedMicroseconds.QuadPart *= 1000000;
				ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
				if (frameCounter * sleepInMicroSeconds <= ElapsedMicroseconds.QuadPart) {
					break;
				}
			}

			frameCounter++;

		}
		m_bvhLock.unlock();

	}

	//std::cout << "ElapsedMicroseconds.QuadPart " << ElapsedMicroseconds.QuadPart << std::endl;

	writeFinalBvhFile();
}



