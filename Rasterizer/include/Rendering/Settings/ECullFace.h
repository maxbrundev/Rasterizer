#pragma once

#include <cstdint>

namespace Rendering::Settings
{
	enum ECullFace : uint8_t
	{
		BACK  = 0b0001'0000,
		FRONT = 0b0010'0000,
	};
}
