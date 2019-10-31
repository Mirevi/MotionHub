#pragma once

#include "ConfigDllExportMotionHubUtil.h"
#include <map>
#include "MotionHubUtil/Console.h"

/*!
 * 
 * \class InputManager
 * 
 * \brief container for input data
 * 
 * \author Kester Evers and Eric Jansen
 * 
 */
class MotionHubUtil_DLL_import_export InputManager
{

public:
	/*!
	 *  default constructor
	 */
	InputManager();
	/*!
	 * 
	 * check if button with id is pressed
	 * 
	 * \param buttonId button id
	 * \return bool is button pressed
	 */
	bool isButtonPressed(int buttonId);
	/*!
	 * 
	 * set button with id pressed
	 * 
	 * \param buttonId button id
	 * \param pressed bool is button pressed
	 */
	void setButtonPressed(int buttonId, bool pressed);
	/*!
	 * 
	 * register button and assign id
	 * 
	 * \param buttonId button id
	 */
	void registerButton(int buttonId);
	/*!
	 * 
	 * get id of selected tracker in create tracker dialog
	 * 
	 * \return tracker id
	 */
	int getSelectedTrackerIdInDropdown();
	/*!
	 * 
	 * set id of selected tracker in create tracker dialog
	 * 
	 * \param id tracker id
	 */
	void setSelectedTrackerIdInDropdown(int id);
	/*!
	 *
	 * get id of selected tracker in main window tracker list
	 *
	 * \return tracker id
	 */
	int getSelectedTrackerIdInList();
	/*!
	 * 
	 * set id of selected tracker in main window tracker list
	 * 
	 * \param id tracker id
	 */
	void setSelectedTrackerIdInList(int id);

private:
	/*!
	 *
	 * \brief Map for managing button input - holds state for each registered button id
	 * 
	 * values: 0 = not pressed, 1 = pressed
	 *
	 */
	std::map<int, int>* m_poolButtonState;
	/*!
	 * toggle for write and read rights
	 */
	bool m_isLocked = true;
	/*!
	 * selected tracker id in create tracker dialog  
	 */
	int m_selectedTrackerIdInDropdown = 0;
	/*!
	 * selected tracker id in main window tracker list 
	 */
	int m_selectedTrackerIdInList;

};