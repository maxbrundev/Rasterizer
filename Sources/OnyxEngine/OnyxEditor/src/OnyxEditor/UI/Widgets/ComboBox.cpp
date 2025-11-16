#include "OnyxEditor/UI/Widgets/ComboBox.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::ComboBox::ComboBox(int p_currentChoice) : DataWidget<int>(CurrentChoice), CurrentChoice(p_currentChoice)
{
}

void OnyxEditor::UI::Widgets::ComboBox::DrawImplementation()
{
	if (Choices.find(CurrentChoice) == Choices.end())
		CurrentChoice = Choices.begin()->first;

	if (ImGui::BeginCombo(m_widgetID.c_str(), Choices[CurrentChoice].c_str()))
	{
		for (const auto& [key, value] : Choices)
		{
			bool selected = key == CurrentChoice;

			if (ImGui::Selectable(value.c_str(), selected))
			{
				if (!selected)
				{
					ImGui::SetItemDefaultFocus();
					CurrentChoice = key;
					ValueChangedEvent.Invoke(CurrentChoice);
					NotifyChange();
				}
			}
		}

		ImGui::EndCombo();
	}
}
