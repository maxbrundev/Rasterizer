#include "AmberRenderer/Buffers/VertexArray.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"

AmberRenderer::Buffers::VertexArray::VertexArray()
{
	AmberGL::GenVertexArrays(1, &m_bufferID);
	Bind();
}

AmberRenderer::Buffers::VertexArray::~VertexArray()
{
	AmberGL::DeleteVertexArrays(1, &m_bufferID);
}

void AmberRenderer::Buffers::VertexArray::Bind() const
{
	AmberGL::BindVertexArray(m_bufferID);
}

void AmberRenderer::Buffers::VertexArray::Unbind() const
{
	AmberGL::BindVertexArray(0);
}

uint32_t AmberRenderer::Buffers::VertexArray::GetID() const
{
	return m_bufferID;
}
