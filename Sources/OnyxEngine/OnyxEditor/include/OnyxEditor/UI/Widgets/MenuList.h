#pragma once

#include <string>

#include "OnyxEditor/Eventing/Event.h"

#include "OnyxEditor/UI/Widgets/Group.h"

namespace OnyxEditor::UI::Widgets
{
	class MenuList : public Group
	{
	public:
		MenuList(const std::string& p_name, bool p_locked = false);
		virtual ~MenuList() override = default;

	protected:
		virtual void DrawImplementation() override;

	public:
		std::string Name;
		bool Locked;
		Eventing::Event<> ClickedEvent;

	private:
		bool m_opened;
	};
}
