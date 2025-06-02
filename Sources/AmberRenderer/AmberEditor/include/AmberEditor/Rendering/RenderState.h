#pragma once

#include "AmberEditor/Rendering/Settings/EComparisonOperand.h"
#include "AmberEditor/Rendering/Settings/ECullFace.h"
#include "AmberEditor/Rendering/Settings/EPolygonMode.h"

namespace AmberEditor::Rendering
{
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
		bool Blending;
		bool Culling;
	};
}
