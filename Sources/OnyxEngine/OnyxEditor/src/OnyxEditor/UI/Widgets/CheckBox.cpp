#include "OnyxEditor/UI/Widgets/CheckBox.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::CheckBox::CheckBox(bool p_value, const std::string& p_label) :
DataWidget<bool>(Value),
Value(p_value),
Label(p_label)
{
}

void OnyxEditor::UI::Widgets::CheckBox::DrawImplementation()
{
	bool previousValue = Value;

	ImGui::Checkbox((Label + m_widgetID).c_str(), &Value);

	if (Value != previousValue)
	{
		ValueChangedEvent.Invoke(Value);
		NotifyChange();
	}
}
