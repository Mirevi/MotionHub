#pragma once

#include "../MotionHubUtil/ChangeObserver.h"

#include <list>
#include <mutex>

namespace facesynthesizing::domain::adapters::gui{
	class ViewModel : public util::ChangeObservable {
	public:
		void attachListener(util::ChangeListener* listener);
		void detachListener(util::ChangeListener* listener);
		void notify();

		std::mutex viewModelMutex;
	private:
		std::list<util::ChangeListener*> allListeners;
	};
}