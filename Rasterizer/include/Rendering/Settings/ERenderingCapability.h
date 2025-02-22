#pragma once

#include <cstdint>

namespace Rendering::Settings
{
	enum ERenderingCapability : uint8_t
	{
		DEPTH_WRITE = 0x01,
		DEPTH_TEST = 0x02,
		CULL_FACE = 0x04,
	};
}
