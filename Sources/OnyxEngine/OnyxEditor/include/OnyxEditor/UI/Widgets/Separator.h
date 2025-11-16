#pragma once

#include "OnyxEditor/UI/Widgets/AWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class Separator : public AWidget
	{
	public:
		Separator() = default;
		virtual ~Separator() override = default;

	protected:
		void DrawImplementation() override;
	};
}