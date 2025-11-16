#pragma once

#include <string>

#include <glm/glm.hpp>

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

namespace OnyxEditor::UI::Widgets
{
	class ChildWindow : public AWidget, public WidgetContainer
	{
	public:
		ChildWindow(const std::string& p_name = "", const glm::vec2& p_size = glm::vec2(0.0f, 0.0f), bool p_border = false, bool p_scrollable = true);
		virtual ~ChildWindow() override = default;

		void Draw() override;

	protected:
		void DrawImplementation() override;

	public:
		std::string Name;
		glm::vec2 Size;
		bool Border;
		bool Scrollable;
	};
}