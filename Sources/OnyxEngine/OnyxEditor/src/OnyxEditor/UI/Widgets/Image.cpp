#include "OnyxEditor/UI/Widgets/Image.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Widgets::Image::Image(uint32_t p_textureID, const glm::vec2& p_size)
: Size(p_size)
{
	TextureID.ID = p_textureID;
}

void OnyxEditor::UI::Widgets::Image::DrawImplementation()
{
	ImGui::Image(TextureID.Raw, ImVec2(Size.x, Size.y));
}
