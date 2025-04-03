#pragma once

#include <glm/glm.hpp>

#include "AmberGL/Data/Color.h"

#include "AmberGL/API/Export.h"

namespace AmberGL::Geometry
{
	struct API_AMBERGL Vertex
	{
		glm::vec3 position;
		glm::vec2 texCoords;
		glm::vec3 normal;

		Data::Color color;

		Vertex() = default;
		Vertex(glm::vec3 p_position, glm::vec2 p_uv = glm::vec3(0.0f), glm::vec3 p_normal = glm::vec3(0.0f)) : position(p_position), texCoords(p_uv), normal(p_normal)
		{
		}

		bool operator==(const Vertex& p_other) const
		{
			return position == p_other.position && texCoords == p_other.texCoords && normal == p_other.normal;
		}
	};
}
