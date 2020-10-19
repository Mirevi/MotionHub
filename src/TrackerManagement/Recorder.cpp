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
	Console::log("Recorder::addSkeletonsToFrame(): ");

	if (isRecording())
	{

		//for (auto itSkeleton = currSkeletons->begin(); itSkeleton != currSkeletons->end(); itSkeleton++)
		//{

		//	m_currFrame.addSkeleton(itSkeleton->second);

		//}
	}
}

void Recorder::nextFrame()
{
	Console::log("Recorder::nextFrame()");

	if (isRecording())
	{


		m_frames.push_back(m_currFrame);
		m_currFrame = RecordingFrame();
	}
}


void Recorder::startRecording()
{
	m_frames.clear();
	m_currFrame = RecordingFrame();
	m_isRecording.store(true);

	Console::log("Recorder::startRecording()");
}

void Recorder::stopRecording()
{
	m_isRecording.store(false);
	Console::log("Recorder::stopRecording()");

}


bool Recorder::isRecording()
{


	return m_isRecording.load();
}



void Recorder::xyz(int i)
{

	m_lock.lock();

	std::cout << "Recorder::xyz(): " << std::to_string(i) << std::endl;

	m_lock.unlock();
}

#pragma endregion







#pragma region Frame

void RecordingFrame::addSkeleton(Skeleton currSkeleton)
{

	skeletons.push_back(currSkeleton);
}


#pragma endregion
