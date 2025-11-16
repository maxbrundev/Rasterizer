#pragma once

namespace OnyxEditor::UI::Panels
{
	struct PanelSettings
	{
		bool Closable = false;
		bool Resizable = true;
		bool Movable = true;
		bool Scrollable = true;
		bool HideBackground = false;
		bool ForceHorizontalScrollbar = false;
		bool ForceVerticalScrollbar = false;
		bool AllowHorizontalScrollbar = false;
		bool BringToFrontOnFocus = true;
		bool Collapsable = false;
		bool AllowInputs = true;
		bool TitleBar = true;
		bool AutoSize = false;
		bool MenuBar = false;
	};
}