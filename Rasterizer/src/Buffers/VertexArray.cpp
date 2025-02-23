#include "Buffers/VertexArray.h"

#include "Rendering/GLRasterizer.h"

Buffers::VertexArray::VertexArray() : index{0}
{
	GLRasterizer::GenVertexArrays(1, &m_bufferID);
	Bind();
}

Buffers::VertexArray::~VertexArray()
{
	GLRasterizer::DeleteVertexArrays(1, &m_bufferID);
}

void Buffers::VertexArray::BindAttribPointer(const unsigned int p_size, const unsigned int p_type, const unsigned int p_normalized, const unsigned int p_stride, void* p_pointer)
{
	GLRasterizer::EnableVertexAttribArray(index);
	GLRasterizer::VertexAttribPointer(index, static_cast<int>(p_size), static_cast<int>(p_type), p_normalized, static_cast<int>(p_stride), static_cast<const void*>(p_pointer));
	index++;
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
