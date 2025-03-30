#include "AmberRenderer/Resources/Mesh.h"

AmberRenderer::Resources::Mesh::Mesh(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex) :
m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
m_indicesCount(static_cast<uint32_t>(p_indices.size())),
m_materialIndex(p_materialIndex)
{
	InitializeBuffers(p_vertices, p_indices);
}

AmberRenderer::Resources::Mesh::~Mesh()
{
	m_vertexArray.Unbind();
}

void AmberRenderer::Resources::Mesh::Bind()
{
	m_vertexArray.Bind();
}

void AmberRenderer::Resources::Mesh::Unbind()
{
	m_vertexArray.Unbind();
}

uint32_t AmberRenderer::Resources::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

uint32_t AmberRenderer::Resources::Mesh::GetVertexCount()
{
	return m_vertexCount;
}

uint32_t AmberRenderer::Resources::Mesh::GetIndexCount()
{
	return m_indicesCount;
}

void AmberRenderer::Resources::Mesh::InitializeBuffers(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
{
	m_vertexArray.Bind();
	m_vertexBuffer = std::make_unique<Buffers::VertexBuffer>(p_vertices.data(), p_vertices.size());
	m_indexBuffer = std::make_unique<Buffers::IndexBuffer>(p_indices.data(), p_indices.size());
	m_vertexArray.Unbind();
}
