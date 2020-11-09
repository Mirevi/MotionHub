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

void Recorder::nextFrame(float duration)
{

	if (m_isRecording.load())
	{

		//Console::log("Recorder::nextFrame(): next Frame!");


		m_currSession->addFrame(m_currFrame, duration);


	}
}


void Recorder::startRecording()
{

	m_currFrame = RecordingFrame();
	m_isRecording.store(true);



	m_currSession = new RecordingSession();

	m_recordingThread = new std::thread(&Recorder::update, this);
	m_recordingThread->detach();

	Console::log("Recorder::startRecording()");
}

void Recorder::stopRecording()
{
	m_isRecording.store(false);

	m_currSession->save();

	Console::log("Recorder::stopRecording()");
}

void Recorder::update()
{

	while (m_isRecording)
	{
		
		Timer::reset();
		std::this_thread::sleep_for(std::chrono::milliseconds(FRAMETIME));
		float duration = Timer::getDuration();

		nextFrame(duration);

		m_currFrame = RecordingFrame();


	}

}

Recorder& Recorder::instance()
{
	static Recorder _instance;
	return _instance;
}



