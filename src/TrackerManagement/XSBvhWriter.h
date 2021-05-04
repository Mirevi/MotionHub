#pragma once

#include "xs_udpserver.h"
#include "xs_scaledatagram.h"
#include <xstypes/xsquaternion.h>
#include <xstypes/xseuler.h>
#include <math.h>
#include<vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <fenv.h> 
#include <windows.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <Dense>
using namespace Eigen;


/*!
 * \class XSBvhWriter
 *
 * \brief Manages Xsens BVH Export
 *
 */

class XSBvhWriter
{

public:

	enum class SkeletonType { Xsens, MMH };

	/*!
	 */
	XSBvhWriter(enum class SkeletonType skeletonType, UdpServer* udpServer, int avatarId);

	/*!
	* default destructor
	*
	*/
	~XSBvhWriter();


	/*!
	*
	*/

	bool isRecording = true;
	bool isWriting = true;



private:

	/*!
	 * the UDP Server
	 *
	 */
	UdpServer* m_UdpServer;




	/*!
	* joint definitions
	*
	*/
	struct BvhJoint {
		int id;
		std::string name;
		int parentId;
		bool endsite;

	};

	std::vector<BvhJoint>* m_joints;

	enum XsJointHierarchy {
		Hips,
		Chest,
		Chest2,
		Chest3,
		Chest4,
		Neck,
		Head,
		RightCollar,
		RightShoulder,
		RightElbow,
		RightWrist,
		LeftCollar,
		LeftShoulder,
		LeftElbow,
		LeftWrist,
		RightHip,
		RightKnee,
		RightAnkle,
		RightToe,
		LeftHip,
		LeftKnee,
		LeftAnkle,
		LeftToe,
		NUM_JOINTS_XSENS
	};

	// MMH Hierarchy with Xsens Joint IDs
	enum MMHJointHierarchy {
		HIPS = 0,
		SPINE = 2,
		CHEST = 4,
		NECK = 5,
		HEAD = 6,
		SHOULDER_L = 11,
		ARM_L = 12,
		FOREARM_L = 13,
		HAND_L = 14,
		SHOULDER_R = 7,
		ARM_R = 8,
		FOREARM_R = 9,
		HAND_R = 10,
		UPLEG_L = 19,
		LEG_L = 20,
		FOOT_L = 21,
		TOE_L = 22,
		UPLEG_R = 15,
		LEG_R = 16,
		FOOT_R = 17,
		TOE_R = 18,
		NUM_JOINTS_MMH
	};


	/*!
	* new thread for recording
	*/

	std::thread* m_recordingThread;

	/*!
	* streams for read and write
	*/

	std::ifstream bvhMotionInputStreamTemp;
	std::ofstream bvhMotionOutputStreamTemp;
	std::ofstream finalBvhStream;

	/*!
	* file names and paths
	*/

	std::string bvhExportFilePath;
	std::string bvhExportFilePathTemp;
	std::string bvhExportPath;
	std::string bvhFileName;
	int m_avatarId;


	/*!
	* Xsens ScaleDatagram for BVH Offsets
	*/
	ScaleDatagram::BvhScaleInformation* bvhScaleInformation;


	/*!
	* BVH parameters
	*/
	int bvhFrameNumber = 0;
	double bvhFrameTime;


	/*!
	* mutex
	*/
	std::mutex m_bvhLock;

	void setupSkeleton(SkeletonType skeletonType);

	void setupBvhParameters();

	void writeFinalBvhFile();

	void writeMotionDataInBvhStream();


};