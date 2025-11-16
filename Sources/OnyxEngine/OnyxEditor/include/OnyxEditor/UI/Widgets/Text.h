#pragma once

#include <string>

#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class Text : public DataWidget<std::string>
	{
	public:
		Text(const std::string& p_content = "");
		virtual ~Text() override = default;

	protected:
		virtual void DrawImplementation() override;

	public:
		std::string Content;
	};
}