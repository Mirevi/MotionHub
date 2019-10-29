#include "Tracker.h"

Tracker::Tracker()
{



}

void Tracker::start()
{

	m_tracking = true;

	trackThread = new std::thread(&Tracker::track, this);
	trackThread->detach();

	//Console::log("Started tracking thread.");

}

void Tracker::stop()
{

	m_tracking = false;

}

void Tracker::track()
{

	stop();

}

void Tracker::shutdown()
{



}