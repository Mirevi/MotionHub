#pragma once

#include "ConfigDllExportMotionHubUtil.h"
#include <map>
#include "MotionHubUtil/Console.h"

class MotionHubUtil_DLL_import_export InputManager
{

public:
	InputManager();
	bool isButtonPressed(int buttonId);
	void setButtonPressed(int buttonId, bool pressed);
	void registerButton(int buttonId);
	int getCurrSelectedTrackerId();
	void setCurrSelectedTrackerId(int id);

private:
	/*!
	 *
	 * \brief Map for managing button input - holds state for each registered button id
	 * 
	 * values: 0 = not pressed, 1 = pressed
	 *
	 */
	std::map<int, int>* poolButtonState;
	bool m_isLocked;
	int m_currSelectedTrackerId = 0;

};