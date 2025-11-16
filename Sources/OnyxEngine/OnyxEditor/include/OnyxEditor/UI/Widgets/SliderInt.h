#pragma once

#include <string>

#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class SliderInt : public DataWidget<int>
	{
	public:
		SliderInt(int p_min = 0, int p_max = 100, int p_value = 0, const std::string& p_label = "", const std::string& p_format = "%d");
		virtual ~SliderInt() override = default;

	protected:
		void DrawImplementation() override;

	public:
		int min;
		int max;
		std::string label;
		std::string format;
		int CurrentValue;
		//TODO: should be widget member
		float width = -1.0f;
	};
}