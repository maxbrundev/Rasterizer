#pragma once

#include <glm/vec3.hpp>

namespace Geometry
{
	struct Plane
	{
		glm::vec3 Normal;
		glm::vec3 Distance;
	};
}
