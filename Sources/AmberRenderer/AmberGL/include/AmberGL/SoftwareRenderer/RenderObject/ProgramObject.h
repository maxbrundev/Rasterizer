#pragma once

#include <cstdint>

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct ProgramObject
	{
		uint32_t ID = 0;
		Programs::AProgram* Program = nullptr;
	};
}
