#include "Recorder.h"




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
		float duration = Timer::getDuration();

		m_currSession->addFrame(m_currFrame, duration);
		m_currFrame = RecordingFrame();
		Timer::reset();

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



