#include "OnyxEditor/UI/Widgets/SliderInt.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::SliderInt::SliderInt(int p_min, int p_max, int p_value, const std::string& p_label, const std::string& p_format) :
DataWidget(CurrentValue),
CurrentValue(p_value),
min(p_min),
max(p_max),
label(p_label),
format(p_format)
{
	CurrentValue = p_value;
}

void OnyxEditor::UI::Widgets::SliderInt::DrawImplementation()
{
	if (width > 0.0f)
	{
		ImGui::SetNextItemWidth(width);
	}
	else if (width < 0.0f)
	{
		ImGui::SetNextItemWidth(-1);
	}

	if (ImGui::SliderInt((label + m_widgetID).c_str(), &m_data, min, max, format.c_str())) NotifyChange();
}
