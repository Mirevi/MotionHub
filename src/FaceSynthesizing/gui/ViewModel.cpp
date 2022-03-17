#include "ViewModel.h"

//TODO implement
namespace facesynthesizing::domain::adapters::gui{
	void ViewModel::attachListener(util::ChangeListener* listener) {
		this->allListeners.push_back(listener);
	}
	void ViewModel::detachListener(util::ChangeListener* listener) {
		this->allListeners.remove(listener);
	}
	void ViewModel::notify() {
		for (auto* listener : this->allListeners)
			listener->changeListenerTriggered(this);
	}
}