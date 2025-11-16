#include "OnyxEditor/UI/Widgets/Text.h"

#include "OnyxEditor/ImGUI/imgui.h"


OnyxEditor::UI::Widgets::Text::Text(const std::string& p_content) : DataWidget(Content), Content(p_content)
{
}

void OnyxEditor::UI::Widgets::Text::DrawImplementation()
{
	ImGui::Text("%s", Content.c_str());
}
