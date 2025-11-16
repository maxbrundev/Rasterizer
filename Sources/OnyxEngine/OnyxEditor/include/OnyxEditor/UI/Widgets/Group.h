#pragma once

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

namespace OnyxEditor::UI::Widgets
{
	class Group : public AWidget, public WidgetContainer
	{
	public:
		Group() = default;
		virtual ~Group() override = default;

	protected:
		virtual void DrawImplementation() override;
	};
}