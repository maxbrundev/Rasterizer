#pragma once

#include <cstdint>

namespace AmberRenderer::Rendering::Settings
{
	enum class EProjectionMode : uint8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
}
