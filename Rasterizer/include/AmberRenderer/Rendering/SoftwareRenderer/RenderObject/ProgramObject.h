#pragma once

#include <cstdint>

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberRenderer::Rendering::SoftwareRenderer::RenderObject
{
	struct ProgramObject
	{
		uint32_t ID = 0;
		Programs::AProgram* Program = nullptr;
	};
}
