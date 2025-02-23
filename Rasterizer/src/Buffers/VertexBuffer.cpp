#include "Buffers/VertexBuffer.h"

#include "Rendering/GLRasterizer.h"

Buffers::VertexBuffer::VertexBuffer(const void* p_data, size_t p_elements)
{
	GLRasterizer::GenBuffers(1, &m_bufferID);
	Bind();
	GLRasterizer::BufferData(GLR_ARRAY_BUFFER, p_elements * sizeof(Geometry::Vertex), p_data, GLR_STATIC_DRAW);
}

Buffers::VertexBuffer::~VertexBuffer()
{
	GLRasterizer::DeleteBuffers(1, &m_bufferID);
}

void Buffers::VertexBuffer::Bind() const
{
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, m_bufferID);
}

void Buffers::VertexBuffer::Unbind() const
{
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, 0);
}

uint32_t Buffers::VertexBuffer::GetID()
{
	return m_bufferID;
}
