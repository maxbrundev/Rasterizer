
#include "OnyxEditor/UI/Widgets/TextColored.h"

#include "OnyxEditor/ImGUI/imgui.h"
#include "OnyxEditor/UI/Tools/Converter.h"

namespace OnyxEditor::UI::Widgets
{
	TextColored::TextColored(const std::string& p_content, const Data::Color& p_color)
		: Text(p_content), Color(p_color)
	{
	}

	void TextColored::DrawImplementation()
	{
		ImVec4 colorVec = UI::Tools::Converter::ToImVec4(Color);
		ImGui::TextColored(colorVec, "%s", Content.c_str());
	}
}
