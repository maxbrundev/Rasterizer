#pragma once

#include "OnyxEditor/UI/UIIL/Common/UIBackend.h"

namespace OnyxEditor::UI::UIIL
{
	struct NoneBackendContext
	{
		bool IsInitialized = false;
	};

	using NoneBackend = OnyxEditor::UI::UIIL::UIBackend<Settings::EUIBackend::NONE, NoneBackendContext>;
}
