#pragma once

#include <string>

#include "OnyxEditor/Eventing/Event.h"

#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class Selectable : public DataWidget<bool>
	{
	public:
		Selectable(const std::string& p_label = "", bool p_selected = false, bool p_disabled = false);
		virtual ~Selectable() override = default;

	protected:
		void DrawImplementation() override;

	public:
		std::string Label;
		bool Selected;
		bool Disabled;

		Eventing::Event<> ClickedEvent;
	};
}