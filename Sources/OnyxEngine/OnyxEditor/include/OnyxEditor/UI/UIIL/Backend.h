#pragma once

#define UI_API_SDL2

#if defined(UI_API_SDL2)
#include "OnyxEditor/UI/UIIL/SDL2Renderer/SDL2Backend.h"
#else
#include "OnyxEditor/UI/UIIL/None/NoneBackend.h"
#endif

namespace OnyxEditor::UI::UIIL
{
#if defined(UI_API_SDL2)
	using Backend = SDL2Backend;
#else
	using Backend = NoneBackend;
#endif
}
