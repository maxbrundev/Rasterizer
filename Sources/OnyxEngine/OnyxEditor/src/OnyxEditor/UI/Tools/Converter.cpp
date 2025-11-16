#include "OnyxEditor/UI/Tools/Converter.h"

ImVec4 OnyxEditor::UI::Tools::Converter::ToImVec4(const Data::Color& p_value)
{
	return ImVec4(
		p_value.r / 255.0f,
		p_value.g / 255.0f,
		p_value.b / 255.0f,
		p_value.a / 255.0f
	);
}

OnyxEditor::Data::Color OnyxEditor::UI::Tools::Converter::ToColor(const ImVec4& p_value)
{
	return Data::Color(
		static_cast<uint8_t>(p_value.x * 255.0f),
		static_cast<uint8_t>(p_value.y * 255.0f),
		static_cast<uint8_t>(p_value.z * 255.0f),
		static_cast<uint8_t>(p_value.w * 255.0f)
	);
}

ImVec2 OnyxEditor::UI::Tools::Converter::ToImVec2(const glm::vec2& p_value)
{
	return ImVec2(p_value.x, p_value.y);
}

glm::vec2 OnyxEditor::UI::Tools::Converter::ToVec2(const ImVec2& p_value)
{
	return glm::vec2(p_value.x, p_value.y);
}
