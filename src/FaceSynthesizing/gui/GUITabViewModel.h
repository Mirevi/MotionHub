#pragma once

#include "GUIViewModel.h"
#include "GUITab.h"

#include <string>

namespace facesynthesizing::domain::adapters::gui {
	class TabViewModel : public ViewModel {
	public:
		bool isLocked = false;
		GUITabType currentTab = GUITabType::CONVERT;

		// User prompts
		bool showDataAlreadyExistsUserPrompt = false;
		std::string dialogMessage = "";
	};
}