#include "ButtonHelper.h"

bool ButtonHelper::getKey(int key) {

	return GetAsyncKeyState(key) & 0x8000;
}

void ButtonHelper::setKeyDown(int key) {

	setKeyState(key, true);
}

void ButtonHelper::setKeyUp(int key) {

	setKeyState(key, false);
}

bool ButtonHelper::getKeyDown(int key) {

	if (getKey(key)) {
		if (!getKeyState(key)) {
			setKeyState(key, true);
			return true;
		}
	}
	else {
		setKeyState(key, false);
	}
	
	return false;
}

bool ButtonHelper::getKeyState(int key) {

	auto keyIterator = lastKeyState.find(key);

	if (keyIterator != lastKeyState.end()) {
		return keyIterator->second;
	}

	return false;
}

void ButtonHelper::setKeyState(int key, bool state) {

	lastKeyState[key] = state;
}