#pragma once

#include <cstdint>

#include "FrameBufferObject.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Defines.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"


namespace AmberRenderer::Rendering::SoftwareRenderer::RenderObject
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
