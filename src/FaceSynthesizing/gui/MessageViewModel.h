#pragma once

#include "ViewModel.h"

#include <vector>
#include <string>

namespace facesynthesizing::domain::adapters::gui {
	class MessageViewModel : public ViewModel {
	public:
		std::vector<std::string> notes;
		std::string statusMessage = "";
	};
}
