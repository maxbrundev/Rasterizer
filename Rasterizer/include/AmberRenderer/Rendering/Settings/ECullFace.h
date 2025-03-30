#pragma once

namespace AmberRenderer::Rendering::Settings
{
	enum class ECullFace : uint8_t
	{
		BACK  = 0,
		FRONT = 1,
		FRONT_AND_BACK = 2
	};
}
