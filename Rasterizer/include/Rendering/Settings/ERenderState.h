#pragma once

#include <cstdint>

namespace Rendering::Settings
{
	enum ERenderState : uint8_t
	{
		DEPTH_WRITE = 0b0000'0001,
		DEPTH_TEST  = 0b0000'0010,
		CULL_FACE   = 0b0000'0100,
	};
}
