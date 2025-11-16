#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	enum class ETextureParameter : uint8_t
	{
		MIN_FILTER,
		MAG_FILTER,
		WRAP_S,
		WRAP_T
	};
}