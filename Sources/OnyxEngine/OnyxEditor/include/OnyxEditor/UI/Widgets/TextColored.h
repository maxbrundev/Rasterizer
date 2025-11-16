#pragma once

#include "OnyxEditor/Data/Color.h"

#include "OnyxEditor/UI/Widgets/Text.h"

namespace OnyxEditor::UI::Widgets
{
	class TextColored : public Text
	{
	public:
		TextColored(const std::string& p_content = "", const Data::Color& p_color = Data::Color(1.0f, 1.0f, 1.0f, 1.0f));
		virtual ~TextColored() override = default;

	protected:
		virtual void DrawImplementation() override;

	public:
		Data::Color Color;
	};
}