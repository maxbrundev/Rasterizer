#pragma once

#include <cstdint>

namespace OnyxEditor::UI::Settings
{
	enum class EUIBackend : std::uint8_t
	{
		NONE,
		SDL2_RENDERER
	};
}
