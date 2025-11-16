#include "OnyxEditor/UI/Widgets/Button.h"

#include "OnyxEditor/ImGUI/imgui.h"
#include "OnyxEditor/ImGUI/imgui_internal.h"

#include "OnyxEditor/UI/Tools/Converter.h"

OnyxEditor::UI::Widgets::Button::Button(const std::string& p_label, const glm::vec2& p_size, bool p_disabled) :
Label(p_label),
Size(p_size),
Disabled(p_disabled)
{
	const auto& style = ImGui::GetStyle();

	IdleBackgroundColor = Tools::Converter::ToColor(style.Colors[ImGuiCol_Button]);
	HoveredBackgroundColor = Tools::Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
	ClickedBackgroundColor = Tools::Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
	TextColor = Tools::Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void OnyxEditor::UI::Widgets::Button::DrawImplementation()
{
	auto& style = ImGui::GetStyle();

	const auto defaultIdleColor = style.Colors[ImGuiCol_Button];
	const auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
	const auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
	const auto defaultTextColor = style.Colors[ImGuiCol_Text];

	style.Colors[ImGuiCol_Button] = Tools::Converter::ToImVec4(IdleBackgroundColor);
	style.Colors[ImGuiCol_ButtonHovered] = Tools::Converter::ToImVec4(HoveredBackgroundColor);
	style.Colors[ImGuiCol_ButtonActive] = Tools::Converter::ToImVec4(ClickedBackgroundColor);
	style.Colors[ImGuiCol_Text] = Tools::Converter::ToImVec4(TextColor);

	ImVec2 defaultCursorPos = ImGui::GetCursorPos();

	if (IsPositionOverride)
	{
		ImGui::SetCursorScreenPos(ImVec2(Position.x, Position.y));
	}

	if (ImGui::ButtonEx((Label + m_widgetID).c_str(), ImVec2(Size.x, Size.y), Disabled ? ImGuiItemFlags_Disabled : 0))
	{
		ClickedEvent.Invoke();
	}

	if (IsPositionOverride)
	{
		ImGui::SetCursorPos(defaultCursorPos);
	}

	style.Colors[ImGuiCol_Button] = defaultIdleColor;
	style.Colors[ImGuiCol_ButtonHovered] = defaultHoveredColor;
	style.Colors[ImGuiCol_ButtonActive] = defaultClickedColor;
	style.Colors[ImGuiCol_Text] = defaultTextColor;
}
