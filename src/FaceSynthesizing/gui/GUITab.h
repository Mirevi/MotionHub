#pragma once
# include <unordered_map>

namespace facesynthesizing::domain::adapters::gui {
	enum class FaceSynthesizingGUITabType {
		CAPTURE,
		CONVERT,
		TRAIN,
		INFERENCE,
	};

	class GUITab {
	public:
		virtual FaceSynthesizingGUITabType getTabType() = 0;
	};
}