#pragma once

#include <glm/glm.hpp>

//TODO: Should inherit from Primitive
namespace AmberGL::Geometry
{
	struct Point
	{
		glm::vec4 ScreenSpaceCoordinate;

		Point() = default;

		Point(const glm::vec4& p_screenSpaceCoordinate) : ScreenSpaceCoordinate(p_screenSpaceCoordinate)
		{
		}
	};
}
