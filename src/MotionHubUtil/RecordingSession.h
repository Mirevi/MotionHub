#pragma once

#include <vector>

#include "Skeleton.h"
#include "tinyxml2.h"


static const char* RECORD_PATH = "./data/";



class MotionHubUtil_DLL_import_export RecordingFrame
{

public:
	RecordingFrame();
	void addSkeleton(Skeleton currSkeleton);
	std::vector<Skeleton> m_skeletons;


private:


};

class MotionHubUtil_DLL_import_export RecordingSession
{
public:
	RecordingSession();
	void addFrame(RecordingFrame frame);
	void save();

private:
	std::vector<RecordingFrame> m_frames;

};
