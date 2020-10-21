#pragma once

#include <vector>
#include <atomic>

#include "MotionHubUtil/Skeleton.h"
#include "ConfigDllExportTrackingManagement.h"
#include "MotionHubUtil/tinyxml2.h"

static const char* RECORD_PATH = "./data/";




class RecordingFrame
{

public:
	void addSkeleton(Skeleton currSkeleton);
	std::vector<Skeleton> m_skeletons;


private:


};

class RecordingSession
{
public:
	RecordingSession();
	void addFrame(RecordingFrame frame);
	void save();

private:
	std::vector<RecordingFrame> m_frames;
	tinyxml2::XMLDocument* m_doc;	//####LINKER ERROR without pointer####
};


class __declspec(dllexport) Recorder
{

public:

	static Recorder& instance();
	~Recorder() {}


	void toggleRecording();
	void addSkeletonsToFrame(std::map<int, Skeleton>* currSkeletons);
	void nextFrame();




private:


	Recorder() {}							// verhindert, dass ein Objekt von außerhalb von Recorder erzeugt wird.
											// protected, wenn man von der Klasse noch erben möchte
	Recorder(const Recorder&);				// verhindert, dass eine weitere Instanz via Kopier-Konstruktor erstellt werden kann
	Recorder& operator = (const Recorder&); //Verhindert weitere Instanz durch Kopie




	//std::mutex m_lock;

	RecordingSession* m_currSession;

	RecordingFrame m_currFrame;

	std::atomic<bool> m_isRecording;




	void startRecording();
	void stopRecording();



};


