#pragma once

#include <vector>

#include "Skeleton.h"
#include "tinyxml2.h"
#include "TrackerManagement/bvh11.h"





class MotionHubUtil_DLL_import_export RecordingFrame
{




public:
	RecordingFrame();
	void addSkeleton(Skeleton currSkeleton);
	std::vector<Skeleton> m_skeletons;



	float m_duration;
private:


};

class MotionHubUtil_DLL_import_export RecordingSession
{
public:
	RecordingSession();
	void addFrame(RecordingFrame frame, float duration);
	void save(int* progression);
	void saveBVH();
	void load(std::string filePath);
	void loadBVH(std::string filePath);

	int getFrameCount();
	RecordingFrame* getFrame(int i);

	static std::string RECORD_PATH;

	float getTotalTime();

private:
	std::vector<RecordingFrame> m_frames;
	float m_totalTime;



};
