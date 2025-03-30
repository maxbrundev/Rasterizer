#pragma once

#include <cstdint>

namespace AmberRenderer::Resources::Settings
{
	enum ETextureWrapMode : uint8_t
	{
		CLAMP,
		REPEAT
	};
}