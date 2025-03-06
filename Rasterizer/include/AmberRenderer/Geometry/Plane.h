#pragma once

#include <glm/vec3.hpp>

namespace AmberRenderer::Geometry
{
	struct Plane
	{
		glm::vec3 Normal;
		float Distance;
	};
}
