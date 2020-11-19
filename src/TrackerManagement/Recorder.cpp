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



void Recorder::addSkeletonsToFrame(std::map<int, Skeleton> currSkeletons)
{

	//std::stringstream ss;
	//ss << static_cast<const void*>(this);
	//Console::log("Recorder::addSkeletonsToFrame(): this = " + ss.str());


	if (m_isRecording.load())
	{
		//Console::log(" Recorder::addSkeletonsToFrame()");

		for (auto itSkeleton = currSkeletons.begin(); itSkeleton != currSkeletons.end(); itSkeleton++)
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
		Timer::reset();

		m_currSession->addFrame(m_currFrame, duration);

		m_currFrame = RecordingFrame();
	}
}


void Recorder::startRecording()
{
	if (!m_isRecording.load())
	{

	
		m_currFrame = RecordingFrame();
		m_isRecording.store(true);
		



		m_currSession = new RecordingSession();

		//m_recordingThread+ = new std::thread(&Recorder::update, this);
		//m_recordingThread->detach();

		Console::log("Recorder::startRecording()");
		Timer::reset();
	}
	else
	{
		Console::logError("Recorder::startRecording(): Recorder is already running");
	}

}

void Recorder::stopRecording()
{
	m_isRecording.store(false);

	m_currSession->save();

	Console::log("Recorder::stopRecording()");
}

void Recorder::update()
{

	//while (m_isRecording)
	//{
	//	
	//	Timer::reset();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(FRAMETIME));
	//	float duration = Timer::getDuration();

	//	//Console::log("duration: " + toString(duration));

	//	nextFrame(duration);

	//	m_currFrame = RecordingFrame();


	//}

}

Recorder& Recorder::instance()
{
	static Recorder _instance;
	return _instance;
}

void Recorder::recodFrame()
{

}

bool Recorder::isRecording()
{
	return m_isRecording.load();
}