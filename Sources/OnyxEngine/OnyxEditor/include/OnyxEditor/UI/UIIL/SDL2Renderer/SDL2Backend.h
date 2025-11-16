#pragma once

#include "OnyxEditor/UI/UIIL/Common/UIBackend.h"

namespace OnyxEditor::UI::UIIL
{
	struct SDL2BackendContext
	{
		void* Window = nullptr;
		void* Renderer = nullptr;
		bool IsInitialized = false;
	};

	using SDL2Backend = OnyxEditor::UI::UIIL::UIBackend<Settings::EUIBackend::SDL2_RENDERER, SDL2BackendContext>;
}
