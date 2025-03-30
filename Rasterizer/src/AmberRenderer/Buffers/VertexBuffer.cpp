#include "AmberRenderer/Buffers/VertexBuffer.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"

AmberRenderer::Buffers::VertexBuffer::VertexBuffer(const void* p_data, size_t p_elements)
{
	AmberGL::GenBuffers(1, &m_bufferID);
	Bind();
	AmberGL::BufferData(AGL_ARRAY_BUFFER, p_elements * sizeof(Geometry::Vertex), p_data);
}

AmberRenderer::Buffers::VertexBuffer::~VertexBuffer()
{
	AmberGL::DeleteBuffers(1, &m_bufferID);
}

void AmberRenderer::Buffers::VertexBuffer::Bind() const
{
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, m_bufferID);
}

void AmberRenderer::Buffers::VertexBuffer::Unbind() const
{
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, 0);
}

uint32_t AmberRenderer::Buffers::VertexBuffer::GetID()
{
	return m_bufferID;
}
