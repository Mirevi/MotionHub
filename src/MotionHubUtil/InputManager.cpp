#include "InputManager.h"

InputManager::InputManager()
{

	m_isLocked = false;

	poolButtonState = new std::map<int, int>;


}

bool InputManager::isButtonPressed(int buttonId)
{
	
	if (!m_isLocked)
	{

		if (!(poolButtonState->find(buttonId) == poolButtonState->end()) && poolButtonState->at(buttonId))
		{

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

void InputManager::setButtonPressed(int buttonId, bool pressed)
{

	m_isLocked = true;

	if (!(poolButtonState->find(buttonId) == poolButtonState->end()))
	{

		poolButtonState->at(buttonId) = pressed;
		//Console::log("Button " + std::to_string(buttonId) + " set to " + std::to_string(pressed));

	}
	else
		Console::logError("Unknown button id = " + std::to_string(buttonId) + ". Button is not registered!");

	m_isLocked = false;

}

void InputManager::registerButton(int buttonId)
{

	poolButtonState->insert( { buttonId, 0 } );

}

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

void InputManager::setSelectedTrackerIdInList(int id) 
{

	m_selectedTrackerIdInList = id;

}