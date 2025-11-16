#pragma once

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include "OnyxEditor/Eventing/Event.h"

namespace OnyxEditor::UI::Widgets
{
	class AButton : public AWidget
	{
	public:
		AButton() = default;
		virtual ~AButton() override = default;

	protected:
		virtual void DrawImplementation() override = 0;

	public:
		Eventing::Event<> ClickedEvent;
	};
}