#pragma once

#include "OnyxEditor/Rendering/Settings/EComparisonOperand.h"
#include "OnyxEditor/Rendering/Settings/ECullFace.h"
#include "OnyxEditor/Rendering/Settings/EPolygonMode.h"

namespace OnyxEditor::Rendering
{
	enum class EStencilOp : uint16_t
	{
		KEEP = 0x0F00,
		ZERO = 0x0F01,
		REPLACE = 0x0F02,
		INCR = 0x0F03,
		INCR_WRAP = 0x0F04,
		DECR = 0x0F05,
		DECR_WRAP = 0x0F06,
		INVERT = 0x0F07
	};

	struct RenderState
	{
		RenderState();

		Settings::ECullFace CullFace;
		Settings::EPolygonMode PolygonMode;
		Settings::EComparisonOperand DepthFunc;

		uint8_t LineWidth;
		uint8_t PointSize;

		bool MultiSample;
		bool DepthTest;
		bool DepthWriting;
		bool ColorWriting;
		bool Blending;
		bool Culling;

		bool StencilTest;
		Settings::EComparisonOperand StencilFunc;
		int StencilRef;
		uint32_t StencilMask;
		uint32_t StencilWriteMask;
		EStencilOp StencilFail;
		EStencilOp StencilPassDepthFail;
		EStencilOp StencilPassDepthPass;
	};
}
