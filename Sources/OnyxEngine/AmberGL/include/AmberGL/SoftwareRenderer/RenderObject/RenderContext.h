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

		float LineWidth = 1.0f;
		float PointSize = 1.0f;

		struct
		{
			uint16_t X = 0;
			uint16_t Y = 0;
			uint16_t Width = 0;
			uint16_t Height = 0;
		} Viewport;

		uint16_t DepthFunc = AGL_LESS;
		uint16_t PolygonMode = AGL_FILL;
		uint16_t CullFace = AGL_BACK;
		uint8_t Samples;

		uint8_t State = AGL_COLOR_WRITE | AGL_DEPTH_WRITE | AGL_DEPTH_TEST | AGL_CULL_FACE;

		uint16_t StencilFunc = AGL_ALWAYS;
		int StencilRef = 0;
		uint32_t StencilMask = 0xFFFFFFFF;
		uint32_t StencilWriteMask = 0xFFFFFFFF;
		uint16_t StencilFail = AGL_KEEP;
		uint16_t StencilPassDepthFail = AGL_KEEP;
		uint16_t StencilPassDepthPass = AGL_KEEP;
	};
}
