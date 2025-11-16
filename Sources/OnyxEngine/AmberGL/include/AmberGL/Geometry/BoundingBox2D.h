#pragma once

#include <glm/vec2.hpp>

namespace AmberGL::Geometry
{
	struct BoundingBox2D
	{
		glm::ivec2 Min;
		glm::ivec2 Max;
	};
}
