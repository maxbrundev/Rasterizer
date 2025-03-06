#include "AmberRenderer/Buffers/VertexArray.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

AmberRenderer::Buffers::VertexArray::VertexArray()
{
	GLRasterizer::GenVertexArrays(1, &m_bufferID);
	Bind();
}

AmberRenderer::Buffers::VertexArray::~VertexArray()
{
	GLRasterizer::DeleteVertexArrays(1, &m_bufferID);
}

void AmberRenderer::Buffers::VertexArray::Bind() const
{
	GLRasterizer::BindVertexArray(m_bufferID);
}

void AmberRenderer::Buffers::VertexArray::Unbind() const
{
	GLRasterizer::BindVertexArray(0);
}

uint32_t AmberRenderer::Buffers::VertexArray::GetID() const
{
	return m_bufferID;
}
