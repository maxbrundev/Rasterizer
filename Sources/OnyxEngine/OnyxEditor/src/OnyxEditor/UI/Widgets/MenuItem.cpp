#include "OnyxEditor/UI/Widgets/MenuItem.h"
#include "OnyxEditor/ImGUI/imgui.h"


OnyxEditor::UI::Widgets::MenuItem::MenuItem(const std::string& p_name, const std::string& p_shortcut, bool p_checkable, bool p_checked) :
name(p_name),
shortcut(p_shortcut),
checkable(p_checkable),
checked(p_checked),
m_selected(false),
m_wasCheckable(p_checkable)
{
}

void OnyxEditor::UI::Widgets::MenuItem::Draw()
{
	if (Enabled)
	{
		DrawImplementation();
		ExecuteBehaviors();
	}
}

void OnyxEditor::UI::Widgets::MenuItem::DrawImplementation()
{
	bool hasChildren = !Widgets.empty();

	if (hasChildren)
	{
		if (ImGui::BeginMenu((name + m_widgetID).c_str(), Enabled))
		{
			DrawWidgets();
			ImGui::EndMenu();
		}
	}
	else
	{
		bool* checkPtr = checkable ? &checked : nullptr;
		const char* shortcutStr = shortcut.empty() ? nullptr : shortcut.c_str();

		if (ImGui::MenuItem((name + m_widgetID).c_str(), shortcutStr, checkPtr, Enabled))
		{
			m_selected = true;
			ClickedEvent.Invoke();
		}

		if (checkable && m_wasCheckable && checked != m_selected)
		{
			ValueChangedEvent.Invoke(checked);
		}

		m_wasCheckable = checkable;
		m_selected = false;
	}
}
