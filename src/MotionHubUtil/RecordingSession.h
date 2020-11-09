#pragma once

#include <vector>

#include "Skeleton.h"
#include "tinyxml2.h"
#include "TrackerManagement/bvh11.h"


static const char* RECORD_PATH = "./data/";



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
	void save();
	void saveBVH();
	void load(std::string filePath);
	void loadBVH(std::string filePath);

	int getFrameCount();
	RecordingFrame* getFrame(int i);

private:
	std::vector<RecordingFrame> m_frames;



};
