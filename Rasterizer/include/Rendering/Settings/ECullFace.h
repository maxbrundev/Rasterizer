#pragma once

#include <cstdint>

namespace Rendering::Settings
{
	enum ECullFace : uint8_t
	{
		BACK  = 0,
		FRONT = 1,
		FRONT_AND_BACK = 2
	};
}
