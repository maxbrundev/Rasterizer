#pragma once

#include <cstdint>
#include <string>

namespace OnyxEditor::Rendering::Debug
{
	struct FrameStep
	{
		std::string Description;
		std::string PassName;
		std::string MaterialName;
		uint32_t TriangleCount = 0;
		uint32_t VertexCount = 0;
		uint32_t CapturedTextureID = 0;
	};
}
