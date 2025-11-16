#pragma once

#include <string>

#include "OnyxEditor/Eventing/Event.h"

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

namespace OnyxEditor::UI::Widgets
{
	class MenuItem : public AWidget, public WidgetContainer
	{
	public:
		MenuItem(const std::string& p_name, const std::string& p_shortcut = "", bool p_checkable = false, bool p_checked = false);
		virtual ~MenuItem() override = default;

		void Draw() override;

	protected:
		void DrawImplementation() override;

	public:
		std::string name;
		std::string shortcut;
		bool checkable;
		bool checked;

		Eventing::Event<> ClickedEvent;
		Eventing::Event<bool> ValueChangedEvent;

	private:
		bool m_selected;
		bool m_wasCheckable;
	};
}