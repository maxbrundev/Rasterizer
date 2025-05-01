#pragma once

#include <array>

#include <glm/glm.hpp>

//TODO: Should inherit from Primitive
namespace AmberGL::Geometry
{
	struct Line
	{
		std::array<glm::vec4, 2> ScreenSpaceCoordinates;
		float Lenght;

		Line(const std::array<glm::vec4, 2>& p_screenSpaceCoordinates) : ScreenSpaceCoordinates(p_screenSpaceCoordinates)
		{
			glm::vec2 delta = ScreenSpaceCoordinates[0] - ScreenSpaceCoordinates[1];
			Lenght = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		}
	};
}
