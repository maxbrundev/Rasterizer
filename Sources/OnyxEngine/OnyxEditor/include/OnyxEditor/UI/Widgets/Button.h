#pragma once

#include "OnyxEditor/UI/Widgets/AButton.h"

#include <string>

#include <glm/glm.hpp>

#include "OnyxEditor/Data/Color.h"

namespace OnyxEditor::UI::Widgets
{
	class Button : public AButton
	{
	public:
		Button(const std::string& p_label = "", const glm::vec2& p_size = glm::vec2(0.0f, 0.0f), bool p_disabled = false);
		virtual ~Button() override = default;

	protected:
		void DrawImplementation() override;

	public:
		std::string Label;

		glm::vec2 Size;
		glm::vec2 Position;

		Data::Color IdleBackgroundColor;
		Data::Color HoveredBackgroundColor;
		Data::Color ClickedBackgroundColor;
		Data::Color TextColor;

		bool Disabled = false;
		bool IsPositionOverride = false;
	};
}