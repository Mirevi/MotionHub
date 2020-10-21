#include "RecordingSession.h"




#pragma region Session

RecordingSession::RecordingSession()
{

}

void RecordingSession::addFrame(RecordingFrame frame)
{
	m_frames.push_back(frame);
}

void RecordingSession::save()
{


	std::string filename = "MMH_" + Timer::getCurrTime() + ".xml";


	//m_doc = new tinyxml2::XMLDocument();

	tinyxml2::XMLDocument doc;

	tinyxml2::XMLNode* pRoot = doc.NewElement("Frames");
	
	doc.InsertFirstChild(pRoot);


	int iF = 0;

	//loop through all frames
	for (auto itFrames = m_frames.begin(); itFrames != m_frames.end(); itFrames++)
	{

		tinyxml2::XMLNode* pFrame = doc.NewElement("Frame_" + iF++);
		pRoot->InsertEndChild(pFrame);

		int iS = 0;

		//loop through all skeletons of a frame
		for (auto itSkeleton = itFrames->m_skeletons.begin(); itSkeleton < itFrames->m_skeletons.end(); itSkeleton++)
		{

			tinyxml2::XMLElement* pSkeleton = doc.NewElement("Skeleton_" + iS++);
			pSkeleton->SetText("Skeleton_" + iS++);
			pFrame->InsertEndChild(pSkeleton);

		}
	}

	filename += RECORD_PATH;

	doc.SaveFile(filename.c_str());

	Console::log("RecordingSession::save(): Session saved as: " + filename);

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
