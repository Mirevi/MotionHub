//#include "InputManager.h"
//
//InputManager::InputManager()
//{
//
//	registerButton(MainWindow::startTracking);
//
//}
//
//bool InputManager::isButtonPressed(MainWindow::Buttons button)
//{
//	
//	if (poolButtonState->at(button) != NULL)
//		return poolButtonState->at(button);
//	else
//		return false;
//
//}
//
//void InputManager::setButtonPressed(MainWindow::Buttons button)
//{
//
//	if (poolButtonState->at(button) != NULL)
//		poolButtonState->at(button) = true;
//
//	poolButtonState->at(button) = false;
//
//}
//
//void InputManager::registerButton(MainWindow::Buttons button)
//{
//
//	poolButtonState->insert({ button, false } );
//
//}