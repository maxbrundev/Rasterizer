#pragma once

#include <array>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Geometry
{
	constexpr int MAX_POLY_COUNT = 12;

	struct Polygon
	{
		std::array<glm::vec4, MAX_POLY_COUNT> Vertices;
		std::array <glm::vec2, MAX_POLY_COUNT> TextCoords;
		std::array <glm::vec3, MAX_POLY_COUNT> Normals;
		uint8_t VerticesCount;
	};
}
