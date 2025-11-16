#include "OnyxEditor/UI/Panels/APanel.h"

void OnyxEditor::UI::Panels::APanel::Draw()
{
	if (Enabled)
	{
		DrawImplementation();
	}
}
