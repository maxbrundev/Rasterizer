#include "Resources/Mesh.h"

#include "Rendering/GLRasterizer.h"

Resources::Mesh::Mesh(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex) :
m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
m_indicesCount(static_cast<uint32_t>(p_indices.size())),
m_materialIndex(p_materialIndex)
{
	InitializeBuffers(p_vertices, p_indices);
}

Resources::Mesh::~Mesh()
{
	/*GLRasterizer::DeleteBuffers(1, &m_VBO);
	GLRasterizer::DeleteBuffers(1, &m_EBO);
	GLRasterizer::DeleteVertexArrays(1, &m_VAO);*/

	m_vertexArray.Unbind();
}

void Resources::Mesh::Bind()
{
	m_vertexArray.Bind();
}

void Resources::Mesh::Unbind()
{
	m_vertexArray.Unbind();
}

const std::vector<Geometry::Vertex>& Resources::Mesh::GetVertices() const
{
	return vertices;
}

uint32_t Resources::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

const std::vector<uint32_t>& Resources::Mesh::GetIndices() const
{
	return indices;
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
	//vertices = p_vertices;
	//indices = p_indices;

	m_vertexBuffer = std::make_unique<Buffers::VertexBuffer>(p_vertices.data(), p_vertices.size());
	m_indexBuffer = std::make_unique<Buffers::IndexBuffer>(p_indices.data(), p_indices.size());
	m_vertexArray.index = 0;
	m_vertexArray.BindAttribPointer(3, GLR_FLOAT, false, sizeof(Geometry::Vertex), (void*)0);
	m_vertexArray.Unbind();
	/*GLRasterizer::GenBuffers(1, &m_VBO);
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, m_VBO);
	GLRasterizer::BufferData(GLR_ARRAY_BUFFER,
		p_vertices.size() * sizeof(Geometry::Vertex),
		p_vertices.data(),
	                         GLR_STATIC_DRAW);


	GLRasterizer::GenBuffers(1, &m_EBO);
	GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, m_EBO);
	GLRasterizer::BufferData(GLR_ELEMENT_ARRAY_BUFFER,
		p_indices.size() * sizeof(uint32_t),
		p_indices.data(),
	                         GLR_STATIC_DRAW);

	GLRasterizer::GenVertexArrays(1, &m_VAO);
	GLRasterizer::BindVertexArray(m_VAO);

	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, m_VBO);
	GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, m_EBO);

	GLRasterizer::EnableVertexAttribArray(0);
	GLRasterizer::VertexAttribPointer(0, 3, GLR_FLOAT, false, sizeof(Geometry::Vertex), (void*)0);

	GLRasterizer::BindVertexArray(0);*/
}
