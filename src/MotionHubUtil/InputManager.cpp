#include "InputManager.h"

// default constructor
InputManager::InputManager()
{

	m_isLocked;
	// create new map with button state and id
	m_poolButtonState = new std::map<int, int>;

	m_trackerPool = new std::map<std::pair<std::string, int>, std::map<int, Skeleton*>*>;

}

// get the state of a button with id
bool InputManager::isButtonPressed(int buttonId)
{
	
	// get state if locked is false
	if (!m_isLocked)
	{

		// check if button exists in pool and is pressed
		if (!(m_poolButtonState->find(buttonId) == m_poolButtonState->end()) && m_poolButtonState->at(buttonId) == 1)
		{

			// reset button state to none
			setButtonPressed(buttonId, 0);
			return true;

		}
		else
			return false;

	}
	else
	{

		//Console::logError("InputManager::isButtonPressed(): Button with id = " + std::to_string(buttonId) + " state can not be read! Input is locked.");

		return false;

	}
}


// set button with id to pressed true / false
void InputManager::setButtonPressed(int buttonId, bool pressed)
{

	// lock in order to write state
	m_isLocked = true;

	// check if button exists in pool
	if (!(m_poolButtonState->find(buttonId) == m_poolButtonState->end()))
	{

		// set state to argument "pressed" true / false
		m_poolButtonState->at(buttonId) = pressed;
		//Console::log("Button " + std::to_string(buttonId) + " set to " + std::to_string(pressed));

	}
	else
		Console::logError("Unknown button id = " + std::to_string(buttonId) + ". Button is not registered!");

	// unlock
	m_isLocked = false;

}

// register button with id in button state pool
void InputManager::registerButton(int buttonId)
{

	// insert button with id in pool and set state to none
	m_poolButtonState->insert( { buttonId, 0 } );

}

// set tracker id selected in create new tracker dialog dropdown
void InputManager::setSelectedTrackerIdInDropdown(int id)
{

	m_selectedTrackerIdInDropdown = id;

}

int InputManager::getSelectedTrackerIdInDropdown()
{

	return m_selectedTrackerIdInDropdown;

}

int InputManager::getSelectedTrackerIdInList()
{

	return m_selectedTrackerIdInList;

}

// set tracker id selected in main window list
void InputManager::setSelectedTrackerIdInList(int id) 
{

	m_selectedTrackerIdInList = id;

}

void InputManager::setTrackerPool(std::map<std::pair<std::string, int>, std::map<int, Skeleton*>*>* trackerPool)
{

	m_trackerPool = trackerPool;

}

std::map<std::pair<std::string, int>, std::map<int, Skeleton*>*>* InputManager::getTrackerPool()
{

	return m_trackerPool;

}

bool InputManager::isTrackerDataAvailable()
{

	if (m_isTrackerDataAvailable)
	{
		m_isTrackerDataAvailable = false;
		return true;
	}
	else
		return false;

}

void InputManager::setTrackerDataAvailable(bool state)
{
	m_isTrackerDataAvailable = state;
}