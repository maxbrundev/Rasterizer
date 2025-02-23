#include "Buffers/VertexArray.h"

#include "Rendering/GLRasterizer.h"

Buffers::VertexArray::VertexArray()
{
	GLRasterizer::GenVertexArrays(1, &m_bufferID);
	Bind();
}

Buffers::VertexArray::~VertexArray()
{
	GLRasterizer::DeleteVertexArrays(1, &m_bufferID);
}

void Buffers::VertexArray::Bind() const
{
	GLRasterizer::BindVertexArray(m_bufferID);
}

void Buffers::VertexArray::Unbind() const
{
	GLRasterizer::BindVertexArray(0);
}

uint32_t Buffers::VertexArray::GetID() const
{
	return m_bufferID;
}
