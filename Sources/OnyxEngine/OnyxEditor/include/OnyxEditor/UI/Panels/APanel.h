#pragma once

#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

namespace OnyxEditor::UI::Panels
{
	class APanel : public  Widgets::WidgetContainer
	{
	public:
		APanel() = default;
		virtual ~APanel() override = default;

		virtual void Draw();

	protected:
		virtual void DrawImplementation() = 0;

	public:
		bool Enabled = true;
	};
}
