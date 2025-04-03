#pragma once

#include <glm/vec3.hpp>

#include "AmberGL/Geometry/BoundingBox2D.h"

namespace AmberGL::Geometry
{
	struct Triangle
	{
		glm::vec2 Vertices[3];

		BoundingBox2D BoundingBox2D;

		Triangle(const glm::vec2& p_vertex0, const glm::vec2& p_vertex1, const glm::vec2& p_vertex2);

		void PreComputeBarycentricCoordinates();
		glm::vec3 GetBarycentricCoordinates(const glm::vec2& p_vertex) const;

		float ComputeEdge(const glm::vec2& p_vertex0, const glm::vec2& p_vertex1, const glm::vec2& p_vertex2) const;

		float ComputeArea() const;

	private:
		glm::vec2 m_vertex0;
		glm::vec2 m_vertex1;

		float m_d00;
		float m_d01;
		float m_d10;
		float m_d11;
		float m_denom;
	};
}
