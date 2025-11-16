#pragma once

#include <cstdint>
#include <vector>

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct UniformBufferObject
	{
		uint32_t ID = 0;
		size_t Size = 0;
		std::vector<uint8_t> Data;
		std::uint32_t BindingPoint = 0;
	};
}
