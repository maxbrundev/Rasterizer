#include "OnyxEditor/UI/Panels/MenuBarContainer.h"

void OnyxEditor::UI::Panels::MenuBarContainer::DrawMenuBar()
{
	for (const auto& widget : m_menuWidgets)
	{
		widget->Draw();
	}
}

void OnyxEditor::UI::Panels::MenuBarContainer::RemoveAllMenuWidgets()
{
	m_menuWidgets.clear();
}