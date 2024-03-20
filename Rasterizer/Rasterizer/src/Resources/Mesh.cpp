#include "Resources/Mesh.h"

void Resources::Mesh::AddVertex(const Geometry::Vertex& p_vertex)
{
	m_vertices.emplace_back(p_vertex);
}

void Resources::Mesh::AddIndice(uint32_t p_indice)
{
	m_indices.emplace_back(p_indice);
}

const std::vector<Geometry::Vertex>& Resources::Mesh::GetVertices() const
{
	return m_vertices;
}

const std::vector<uint32_t>& Resources::Mesh::GetIndices() const
{
	return m_indices;
}
