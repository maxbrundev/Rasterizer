#pragma once

#include <cstdint>

#include "AmberGL/SoftwareRenderer/Defines.h"
#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"
#include "AmberGL/SoftwareRenderer/RenderObject/FrameBufferObject.h"

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct RenderContext
	{
		uint16_t ViewPortX = 0;
		uint16_t ViewPortY = 0;
		uint16_t ViewPortWidth = 0;
		uint16_t ViewPortHeight = 0;
		uint8_t State = 0;
		uint8_t PolygonMode = AGL_FILL;
		uint8_t CullFace = AGL_BACK;
		uint8_t Samples = 0;
		FrameBufferObject* FrameBufferObject;
		Programs::AProgram* Program = nullptr;
	};
}
