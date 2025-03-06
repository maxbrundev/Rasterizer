#include "AmberRenderer/Buffers/VertexBuffer.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

AmberRenderer::Buffers::VertexBuffer::VertexBuffer(const void* p_data, size_t p_elements)
{
	GLRasterizer::GenBuffers(1, &m_bufferID);
	Bind();
	GLRasterizer::BufferData(GLR_ARRAY_BUFFER, p_elements * sizeof(Geometry::Vertex), p_data);
}

AmberRenderer::Buffers::VertexBuffer::~VertexBuffer()
{
	GLRasterizer::DeleteBuffers(1, &m_bufferID);
}

void AmberRenderer::Buffers::VertexBuffer::Bind() const
{
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, m_bufferID);
}

void AmberRenderer::Buffers::VertexBuffer::Unbind() const
{
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, 0);
}

uint32_t AmberRenderer::Buffers::VertexBuffer::GetID()
{
	return m_bufferID;
}
