#include "OnyxEditor/UI/Widgets/ChildWindow.h"

#include "OnyxEditor/ImGUI/imgui.h"


OnyxEditor::UI::Widgets::ChildWindow::ChildWindow(const std::string& p_name, const glm::vec2& p_size, bool p_border, bool p_scrollable) :
Name(p_name),
Size(p_size),
Border(p_border),
Scrollable(p_scrollable)
{
}

void OnyxEditor::UI::Widgets::ChildWindow::Draw()
{
	if (Enabled)
	{
		DrawImplementation();
	}
}

void OnyxEditor::UI::Widgets::ChildWindow::DrawImplementation()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

	if (!Scrollable)
	{
		flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	}

	std::string childID = Name.empty() ? m_widgetID : (Name + m_widgetID);

	if (ImGui::BeginChild(childID.c_str(), ImVec2(Size.x, Size.y), Border, flags))
	{
		DrawWidgets();
	}
	ImGui::EndChild();
}
