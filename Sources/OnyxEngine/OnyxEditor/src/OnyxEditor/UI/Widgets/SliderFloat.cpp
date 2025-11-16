#include "OnyxEditor/UI/Widgets/SliderFloat.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::SliderFloat::SliderFloat(float p_min, float p_max, float p_value, const std::string& p_label, const std::string& p_format) :
DataWidget(CurrentValue),
CurrentValue(p_value),
min(p_min),
max(p_max),
label(p_label),
format(p_format)
{
	CurrentValue = p_value;
}

void OnyxEditor::UI::Widgets::SliderFloat::DrawImplementation()
{
	if (width > 0.0f)
	{
		ImGui::SetNextItemWidth(width);
	}
	else if (width < 0.0f)
	{
		ImGui::SetNextItemWidth(-1);
	}

	if (ImGui::SliderFloat((label + m_widgetID).c_str(), &m_data, min, max, format.c_str())) NotifyChange();
}
