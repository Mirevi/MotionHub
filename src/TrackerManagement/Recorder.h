#pragma once

#include <vector>
#include <atomic>

#include "MotionHubUtil/Skeleton.h"
#include "ConfigDllExportTrackingManagement.h"




class RecordingFrame
{

public:
	void addSkeleton(Skeleton currSkeleton);


private:

	std::vector<Skeleton> skeletons;

};




class __declspec(dllexport) Recorder
{

public:

	static Recorder& instance()
	{
		static Recorder _instance;
		return _instance;
	}
	~Recorder() {}
	void xyz(int i);


	void toggleRecording();
	void addSkeletonsToFrame(std::map<int, Skeleton>* currSkeletons);
	void nextFrame();

	bool isRecording();



private:


	Recorder() {}							// verhindert, dass ein Objekt von außerhalb von N erzeugt wird.
											// protected, wenn man von der Klasse noch erben möchte
	Recorder(const Recorder&);				/* verhindert, dass eine weitere Instanz via Kopier-Konstruktor erstellt werden kann */
	Recorder& operator = (const Recorder&); //Verhindert weitere Instanz durch Kopie

	std::mutex m_lock;




	std::vector<RecordingFrame> m_frames;

	RecordingFrame m_currFrame;

	std::atomic<bool> m_isRecording;



	void startRecording();
	void stopRecording();



};


