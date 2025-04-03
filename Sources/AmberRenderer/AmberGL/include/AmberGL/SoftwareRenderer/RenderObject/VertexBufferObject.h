#pragma once

#include <cstdint>
#include <vector>

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct BufferObject
	{
		uint32_t ID = 0;
		uint32_t Target = 0;
		size_t Size = 0;
		std::vector<uint8_t> Data;
	};
}
