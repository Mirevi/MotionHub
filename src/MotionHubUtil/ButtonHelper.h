#pragma once

#include "ConfigDllExportMotionHubUtil.h"
#include "Console.h"

#include <unordered_map>

class MotionHubUtil_DLL_import_export ButtonHelper {
public:

	bool getKey(int key);

	void setKeyDown(int key);

	void setKeyUp(int key);

	bool getKeyDown(int key);

	//void reset();

private:

	bool getKeyState(int key);

	void setKeyState(int key, bool state);

	std::unordered_map<int, bool> lastKeyState;
};
