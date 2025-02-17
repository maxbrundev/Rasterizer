#include "Resources/Mesh.h"

Resources::Mesh::Mesh(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex) :
m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
m_indicesCount(static_cast<uint32_t>(p_indices.size())),
m_materialIndex(p_materialIndex)
{
	InitializeBuffers(p_vertices, p_indices);
}

const std::vector<Geometry::Vertex>& Resources::Mesh::GetVertices() const
{
	return m_vertexBuffer->Vertices;
}

uint32_t Resources::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

const std::vector<uint32_t>& Resources::Mesh::GetIndices() const
{
	return m_indexBuffer->Indices;
}

uint32_t Resources::Mesh::GetVertexCount()
{
	return m_vertexCount;
}

uint32_t Resources::Mesh::GetIndexCount()
{
	return m_indicesCount;
}

void Resources::Mesh::InitializeBuffers(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
{
	m_vertexBuffer = std::make_unique<Buffers::VertexBuffer>(p_vertices);
	m_indexBuffer  = std::make_unique<Buffers::IndexBuffer>(p_indices);
}
