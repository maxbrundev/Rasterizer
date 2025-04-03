#pragma once

#include <glm/vec3.hpp>

namespace AmberGL::Geometry
{
	struct Plane
	{
		glm::vec3 Normal;
		float Distance;
	};
}
