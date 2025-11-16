#pragma once

#include <map>

#include "OnyxEditor/UI/Widgets/DataWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class ComboBox : public DataWidget<int>
	{
	public:
		ComboBox(int p_currentChoice = 0);

	protected:
		void DrawImplementation() override;
\
	public:
		std::map<int, std::string> Choices;
		int CurrentChoice;
		Eventing::Event<int> ValueChangedEvent;
	};
}
