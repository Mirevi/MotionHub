#pragma once

#include <vector>
#include <atomic>
#include <thread>

#include "ConfigDllExportTrackingManagement.h"
#include "MotionHubUtil/RecordingSession.h"

//recording frametime in milliseconds
static int FRAMETIME = 10;




class __declspec(dllexport) Recorder
//class /*TrackingManagement_DLL_import_export*/ Recorder
{

public:

	static Recorder& instance();
	~Recorder() {}


	void addSkeletonsToFrame(std::map<int, Skeleton> currSkeletons);
	void nextFrame();
	bool isRecording();

	void startRecording();
	void stopRecording(int* progression);

	int getFrameCount();



private:


	Recorder() {}							// verhindert, dass ein Objekt von außerhalb von Recorder erzeugt wird.
											// protected, wenn man von der Klasse noch erben möchte

	Recorder(const Recorder&);				// verhindert, dass eine weitere Instanz via Kopier-Konstruktor erstellt werden kann
	Recorder& operator = (const Recorder&); // Verhindert weitere Instanz durch Kopie




	//std::mutex m_lock;

	RecordingSession* m_currSession;

	RecordingFrame m_currFrame;

	std::atomic<bool> m_isRecording;






	void update();

	void recordFrame();


	std::thread* m_recordingThread;

};


