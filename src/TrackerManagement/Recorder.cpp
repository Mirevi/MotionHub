#include "Recorder.h"

#pragma region Recorder




void Recorder::toggleRecording()
{
	if (m_isRecording.load())
	{
		stopRecording();
	}
	else
	{
		startRecording();
	}
}



void Recorder::addSkeletonsToFrame(std::map<int, Skeleton>* currSkeletons)
{

	//std::stringstream ss;
	//ss << static_cast<const void*>(this);
	//Console::log("Recorder::addSkeletonsToFrame(): this = " + ss.str());


	if (m_isRecording.load())
	{
		//Console::log(" Recorder::addSkeletonsToFrame()");

		for (auto itSkeleton = currSkeletons->begin(); itSkeleton != currSkeletons->end(); itSkeleton++)
		{

			m_currFrame.addSkeleton(itSkeleton->second);
			//Console::log(" Recorder::addSkeletonsToFrame(): added akeleton");

		}
	}
	//else
	//{
	//	Console::log(" Recorder::addSkeletonsToFrame(): FALSE");

	//}
}

void Recorder::nextFrame()
{

	if (m_isRecording.load())
	{

		//Console::log("Recorder::nextFrame(): next Frame!");

		

		m_currSession->addFrame(m_currFrame);
		m_currFrame = RecordingFrame();
	}
}


void Recorder::startRecording()
{

	//std::stringstream ss;
	//ss << static_cast<const void*>(this);
	//Console::log("Recorder::startRecording(): this = " + ss.str());


	m_currFrame = RecordingFrame();
	m_isRecording.store(true);



	m_currSession = new RecordingSession();

	Console::log("Recorder::startRecording()");
}

void Recorder::stopRecording()
{
	m_isRecording.store(false);

	m_currSession->save();

	Console::log("Recorder::stopRecording()");
}


Recorder& Recorder::instance()
{
	static Recorder _instance;
	return _instance;
}





#pragma endregion




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

	//tinyxml2::XMLDocument doc;

	//tinyxml2::XMLNode* pRoot = doc.NewElement("Frames");
	//
	//doc.InsertFirstChild(pRoot);


	//int iF = 0;

	//loop through all frames
	//for (auto itFrames = m_frames.begin(); itFrames != m_frames.end(); itFrames++)
	//{

	//	tinyxml2::XMLNode* pFrame = doc.NewElement("Frame_" + iF++);
	//	pRoot->InsertEndChild(pFrame);

	//	int iS = 0;

	//	loop through all skeletons of a frame
	//	for (auto itSkeleton = itFrames->m_skeletons.begin(); itSkeleton < itFrames->m_skeletons.end(); itSkeleton++)
	//	{

	//		tinyxml2::XMLElement* pSkeleton = doc.NewElement("Skeleton_" + iS++);
	//		pSkeleton->SetText("Skeleton_" + iS++);
	//		pFrame->InsertEndChild(pSkeleton);

	//	}
	//}

	//filename += RECORD_PATH;

	//doc.SaveFile(filename.c_str());

	Console::log("RecordingSession::save(): Session saved as: " + filename);

}



#pragma endregion



#pragma region Frame

void RecordingFrame::addSkeleton(Skeleton currSkeleton)
{

	m_skeletons.push_back(currSkeleton);
}


#pragma endregion
