#include "OnyxEditor/Resources/Mesh.h"

OnyxEditor::Resources::Mesh::Mesh(const std::vector<AmberGL::Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex) :
m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
m_indicesCount(static_cast<uint32_t>(p_indices.size())),
m_materialIndex(p_materialIndex)
{
	InitializeBuffers(p_vertices, p_indices);
}

OnyxEditor::Resources::Mesh::~Mesh()
{
	m_vertexArray.Unbind();
}

void OnyxEditor::Resources::Mesh::Bind()
{
	m_vertexArray.Bind();
}

void OnyxEditor::Resources::Mesh::Unbind()
{
	m_vertexArray.Unbind();
}

uint32_t OnyxEditor::Resources::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

uint32_t OnyxEditor::Resources::Mesh::GetVertexCount()
{
	return m_vertexCount;
}

uint32_t OnyxEditor::Resources::Mesh::GetIndexCount()
{
	return m_indicesCount;
}

void OnyxEditor::Resources::Mesh::InitializeBuffers(const std::vector<AmberGL::Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices)
{
	m_vertexArray.Bind();
	m_vertexBuffer = std::make_unique<Buffers::VertexBuffer>(p_vertices.data(), p_vertices.size());
	m_indexBuffer = std::make_unique<Buffers::IndexBuffer>(p_indices.data(), p_indices.size());
	m_vertexArray.Unbind();
}
