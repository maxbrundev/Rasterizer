#pragma once

#include <cstdint>

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"
#include "AmberGL/SoftwareRenderer/RenderObject/FrameBufferObject.h"

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct RenderContext
	{
		FrameBufferObject* FrameBufferObject;
		Programs::AProgram* Program;

		float LineWidth;
		float PointSize;

		uint16_t ViewPortX;
		uint16_t ViewPortY;

		uint16_t ViewPortWidth;
		uint16_t ViewPortHeight;

		uint16_t DepthFunc;
		uint8_t State;
		uint8_t PolygonMode;
		uint8_t CullFace;
		uint8_t Samples;
	};
}
