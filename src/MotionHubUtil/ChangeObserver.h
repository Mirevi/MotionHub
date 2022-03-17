#pragma once

namespace util {
	class ChangeListener;

	class ChangeObservable {
	public:
		virtual void attachListener(ChangeListener* listener) = 0;
		virtual void detachListener(ChangeListener* listener) = 0;
		virtual void notify() = 0;
	};

	class ChangeListener {
	public:
		virtual void changeListenerTriggered(ChangeObservable* source) = 0;
	};
}