#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class ETextureFilteringMode : uint8_t
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR
	};
}