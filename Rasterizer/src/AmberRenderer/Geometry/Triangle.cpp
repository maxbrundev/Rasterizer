#include "AmberRenderer/Geometry/Triangle.h"

#include <algorithm>
#include <cmath>
#include <utility>

AmberRenderer::Geometry::Triangle::Triangle(const glm::vec2& p_vertex0, const glm::vec2& p_vertex1, const glm::vec2& p_vertex2) : Vertices{ p_vertex0, p_vertex1, p_vertex2 }
{
	PreComputeBarycentricCoordinates();

	const float minX = std::min({ p_vertex0.x, p_vertex1.x, p_vertex2.x });
	const float maxX = std::max({ p_vertex0.x, p_vertex1.x, p_vertex2.x });

	const float minY = std::min({ p_vertex0.y, p_vertex1.y, p_vertex2.y });
	const float maxY = std::max({ p_vertex0.y, p_vertex1.y, p_vertex2.y });

	BoundingBox2D.Min.x = static_cast<int32_t>(std::floor(minX));
	BoundingBox2D.Min.y = static_cast<int32_t>(std::floor(minY));
	BoundingBox2D.Max.x = static_cast<int32_t>(std::ceil(maxX));
	BoundingBox2D.Max.y = static_cast<int32_t>(std::ceil(maxY));
}

// Christer Ericson's Real-Time Collision Detection book adaptation.
void AmberRenderer::Geometry::Triangle::PreComputeBarycentricCoordinates()
{
	m_vertex0 = glm::vec2(Vertices[2].x - Vertices[0].x, Vertices[2].y - Vertices[0].y);
	m_vertex1 = glm::vec2(Vertices[1].x - Vertices[0].x, Vertices[1].y - Vertices[0].y);
	m_d00 = m_vertex0.x * m_vertex0.x + m_vertex0.y * m_vertex0.y;
	m_d01 = m_vertex0.x * m_vertex1.x + m_vertex0.y * m_vertex1.y;
	m_d10 = m_vertex1.x * m_vertex0.x + m_vertex1.y * m_vertex1.y;
	m_d11 = m_vertex1.x * m_vertex1.x + m_vertex1.y * m_vertex1.y;
	m_denom = 1.0f / (m_d00 * m_d11 - m_d01 * m_d01);
}

glm::vec3 AmberRenderer::Geometry::Triangle::GetBarycentricCoordinates(const glm::vec2& p_vertex) const
{
	const float x = p_vertex.x - Vertices[0].x;
	const float y = p_vertex.y - Vertices[0].y;

	const float d02 = m_vertex0.x * x + m_vertex0.y * y;
	const float d12 = m_vertex1.x * x + m_vertex1.y * y;

	const float u = (m_d00 * d12 - m_d01 * d02) * m_denom;
	const float v = (m_d11 * d02 - m_d01 * d12) * m_denom;

	return glm::vec3(1.0f - u - v, u, v);
}

float AmberRenderer::Geometry::Triangle::ComputeEdge(const glm::vec2& p_vertex0, const glm::vec2& p_vertex1, const glm::vec2& p_vertex2) const
{
	return (p_vertex1.x - p_vertex0.x) * (p_vertex2.y - p_vertex0.y) - (p_vertex1.y - p_vertex0.y) * (p_vertex2.x - p_vertex0.x);
}

float AmberRenderer::Geometry::Triangle::ComputeArea() const
{
	return ComputeEdge(Vertices[0], Vertices[1], Vertices[2]);
}
