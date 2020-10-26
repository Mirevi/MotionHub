#include "RecordingSession.h"




#pragma region Session

RecordingSession::RecordingSession()
{

}

void RecordingSession::addFrame(RecordingFrame frame, float duration)
{
	if (m_frames.size() > 0)
	{
		m_frames.back().m_duration = duration;
	}


	m_frames.push_back(frame);
}

void RecordingSession::save()
{

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* pRoot = doc.NewElement("Frames");
	
	doc.InsertFirstChild(pRoot);

	int iF = 0;


	//loop through all frames
	for (auto itFrames = m_frames.begin(); itFrames != m_frames.end(); itFrames++)
	{

		std::string str = "Frame_" + std::to_string(iF++);
		const char* chr = str.c_str();

		tinyxml2::XMLNode* pFrame = doc.NewElement(chr);


		int iS = 0;

		//loop through all skeletons of a frame
		for (auto itSkeleton = itFrames->m_skeletons.begin(); itSkeleton < itFrames->m_skeletons.end(); itSkeleton++)
		{

			std::string strs = "Skeleton_" + std::to_string(iS++);
			const char* chrs = strs.c_str();

			tinyxml2::XMLElement* pSkeleton = doc.NewElement(chrs);

			//loop through joints
			for (auto itJoint = itSkeleton->m_joints.begin(); itJoint != itSkeleton->m_joints.end(); itJoint++)
			{

				std::string strj = "Joint_" + std::to_string(itJoint->first);
				const char* chrj = strj.c_str();

				tinyxml2::XMLElement* pJoint = doc.NewElement(chrj);

				

				//insert position
				tinyxml2::XMLElement* pPosition = doc.NewElement("position");

				tinyxml2::XMLElement* pPosX = doc.NewElement("x");
				tinyxml2::XMLElement* pPosY = doc.NewElement("y");
				tinyxml2::XMLElement* pPosZ = doc.NewElement("z");

				pPosX->SetText(std::to_string(itJoint->second.getJointPosition().x()).c_str());
				pPosY->SetText(std::to_string(itJoint->second.getJointPosition().y()).c_str());
				pPosZ->SetText(std::to_string(itJoint->second.getJointPosition().z()).c_str());

				pPosition->InsertEndChild(pPosX);
				pPosition->InsertEndChild(pPosY);
				pPosition->InsertEndChild(pPosZ);

				pJoint->InsertEndChild(pPosition);

				//insert rotation
				tinyxml2::XMLElement* pRotation = doc.NewElement("rotation");

				tinyxml2::XMLElement* pRotX = doc.NewElement("x");
				tinyxml2::XMLElement* pRotY = doc.NewElement("y");
				tinyxml2::XMLElement* pRotZ = doc.NewElement("z");
				tinyxml2::XMLElement* pRotW = doc.NewElement("w");


				pRotX->SetText(std::to_string(itJoint->second.getJointRotation().x()).c_str());
				pRotY->SetText(std::to_string(itJoint->second.getJointRotation().y()).c_str());
				pRotZ->SetText(std::to_string(itJoint->second.getJointRotation().z()).c_str());
				pRotW->SetText(std::to_string(itJoint->second.getJointRotation().w()).c_str());

				pRotation->InsertEndChild(pRotX);
				pRotation->InsertEndChild(pRotY);
				pRotation->InsertEndChild(pRotZ);
				pRotation->InsertEndChild(pRotW);


				pJoint->InsertEndChild(pRotation);



				//insert Confidence
				tinyxml2::XMLElement* pConfidence = doc.NewElement("confidence");
				pConfidence->SetText(std::to_string(itJoint->second.getJointConfidence()).c_str());
				pJoint->InsertEndChild(pConfidence);




				pSkeleton->InsertEndChild(pJoint);

			}



			pFrame->InsertEndChild(pSkeleton);

		}

		//insert frameTime
		tinyxml2::XMLElement* pFrameTime = doc.NewElement("frametime");
		pFrameTime->SetText(std::to_string(itFrames->m_duration).c_str());
		pFrame->InsertEndChild(pFrameTime);

		pRoot->InsertEndChild(pFrame);

	}





	std::string filename = "MMH_" + Timer::getCurrTime() + ".mmh";
	std::string pathStr = RECORD_PATH;
	filename = pathStr + filename;

	filename = removeChar(filename, ' ');
	filename = removeChar(filename, ':');

	tinyxml2::XMLError error = doc.SaveFile(filename.c_str());



	if ((int)error == 0)
	{
		Console::log("RecordingSession::save(): Session saved as: " + filename);
	}
	else
	{
		Console::logError("RecordingSession::save(): Error code " + std::to_string((int)error));
	}


}



#pragma endregion



#pragma region Frame

RecordingFrame::RecordingFrame()
{



}



void RecordingFrame::addSkeleton(Skeleton currSkeleton)
{

	m_skeletons.push_back(currSkeleton);
}


#pragma endregion
