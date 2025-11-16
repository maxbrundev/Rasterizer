#include "OnyxEditor/UI/Widgets/Selectable.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::Selectable::Selectable(const std::string& p_label, bool p_selected, bool p_disabled) :
DataWidget(Selected),
Label(p_label),
Selected(p_selected),
Disabled(p_disabled)
{
}

void OnyxEditor::UI::Widgets::Selectable::DrawImplementation()
{
	bool previousValue = Selected;

	if (ImGui::Selectable((Label + m_widgetID).c_str(), &Selected, Disabled ? ImGuiSelectableFlags_Disabled : 0))
	{
		ClickedEvent.Invoke();
		NotifyChange();
	}

	if (Selected != previousValue)
	{
		ValueChangedEvent.Invoke(Selected);
	}
}
