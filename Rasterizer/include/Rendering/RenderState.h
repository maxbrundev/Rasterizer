#pragma once

#include <cstdint>

#include "Rendering/Settings/ECullFace.h"

namespace Rendering
{
	struct RenderState
	{
		uint8_t DepthTest : 1;
		uint8_t DepthWrite : 1;
		Settings::ECullFace CullFace : 2;

		RenderState() : DepthTest(true), DepthWrite(true), CullFace(Settings::ECullFace::BACK)
		{
		}
	};
}
