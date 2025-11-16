#pragma once

#include <string>

#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class SliderFloat : public DataWidget<float>
	{
	public:
		SliderFloat(float p_min = 0.0f, float p_max = 100.0f, float p_value = 0.0f, const std::string& p_label = "", const std::string& p_format = "%.1");
		virtual ~SliderFloat() override = default;

	protected:
		void DrawImplementation() override;

	public:
		float min;
		float max;
		std::string label;
		std::string format;
		float CurrentValue;
		//TODO: should be widget member
		float width = 0.0f;
	};
}