#pragma once

#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace AmberGL::Geometry
{
	constexpr int MAX_POLY_COUNT = 12;

	struct Polygon
	{
		std::array<glm::vec4, MAX_POLY_COUNT> Vertices;
		std::array <glm::vec2, MAX_POLY_COUNT> TexCoords;
		std::array <glm::vec3, MAX_POLY_COUNT> Normals;

		float Varyings[MAX_POLY_COUNT][16];
		uint8_t VaryingsDataSize;

		uint8_t VerticesCount;
	};
}
