#pragma once

#include <glm/vec3.hpp>

namespace Geometry
{
	struct Plane
	{
		glm::vec3 Normal;
		float Distance;
	};
}
