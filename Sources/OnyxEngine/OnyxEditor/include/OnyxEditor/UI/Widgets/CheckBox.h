#pragma once
#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class CheckBox : public DataWidget<bool>
	{
	public:
		CheckBox(bool p_value = false, const std::string& p_label = "");

	protected:
		void DrawImplementation() override;

	public:
		bool Value;
		std::string Label;
		Eventing::Event<bool> ValueChangedEvent;
	};
}
