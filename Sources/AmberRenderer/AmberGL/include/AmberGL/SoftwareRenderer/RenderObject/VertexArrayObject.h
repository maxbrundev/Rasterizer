#pragma once

#include <cstdint>

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct VertexArrayObject
	{
		uint32_t ID = 0;
		uint32_t BoundArrayBuffer = 0;
		uint32_t BoundElementBuffer = 0;
	};
}
