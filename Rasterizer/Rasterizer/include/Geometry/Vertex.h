#pragma once

#include <glm/glm.hpp>

#include "Data/Color.h"

namespace Geometry
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 textCoords;
		glm::vec3 normal;

		Data::Color color;

		Vertex() = default;
		Vertex(glm::vec3 p_position, glm::vec2 p_uv = glm::vec3(0.0f), glm::vec3 p_normal = glm::vec3(0.0f)) : position(p_position), textCoords(p_uv), normal(p_normal)
		{
		}
	};
}
