#pragma once

#include "ViewModel.h"
#include "GUITab.h"

namespace facesynthesizing::domain::adapters::gui {
	class TabViewModel : public ViewModel {
	public:
		bool isLocked = false;
		FaceSynthesizingGUITabType currentTab = FaceSynthesizingGUITabType::CONVERT;
	};
}