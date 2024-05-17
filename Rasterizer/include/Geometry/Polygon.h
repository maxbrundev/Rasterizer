#pragma once

#include <array>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Geometry
{
	constexpr int MAX_POLY_VERTICES_COUNT = 12;
	constexpr int MAX_POLY_TEXTCOORDS_COUNT = 12;

	struct Polygon
	{
		std::array<glm::vec4, MAX_POLY_VERTICES_COUNT> Vertices;
		std::array <glm::vec2, MAX_POLY_TEXTCOORDS_COUNT> TextCoords;
		uint8_t VerticesCount;
	};
}
