#pragma once
# include <unordered_map>

namespace facesynthesizing::domain::adapters::gui {
	enum class GUITabType {
		CAPTURE,
		CONVERT,
		TRAINING,
		INFERENCE,
	};

	class GUITab {
	public:
		virtual GUITabType getTabType() = 0;
	};
}