
#include "AmberRenderer/Buffers/IndexBuffer.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

AmberRenderer::Buffers::IndexBuffer::IndexBuffer(const unsigned int* p_data, size_t p_elements)
{
	GLRasterizer::GenBuffers(1, &m_bufferID);
	Bind();
	GLRasterizer::BufferData(GLR_ELEMENT_ARRAY_BUFFER, p_elements * sizeof(uint32_t), p_data);
}

AmberRenderer::Buffers::IndexBuffer::~IndexBuffer()
{
	GLRasterizer::DeleteBuffers(1, &m_bufferID);
}

void AmberRenderer::Buffers::IndexBuffer::Bind() const
{
	GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, m_bufferID);
}

void AmberRenderer::Buffers::IndexBuffer::Unbind() const
{
	GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, m_bufferID);
}

uint32_t AmberRenderer::Buffers::IndexBuffer::GetID() const
{
	return m_bufferID;
}
