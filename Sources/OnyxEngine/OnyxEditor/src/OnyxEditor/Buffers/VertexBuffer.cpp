#include "OnyxEditor/Buffers/VertexBuffer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "AmberGL/Geometry/Vertex.h"

OnyxEditor::Buffers::VertexBuffer::VertexBuffer(const void* p_data, size_t p_elements)
{
	AmberGL::GenBuffers(1, &m_bufferID);
	Bind();
	AmberGL::BufferData(AGL_ARRAY_BUFFER, p_elements * sizeof(AmberGL::Geometry::Vertex), p_data);
}

OnyxEditor::Buffers::VertexBuffer::~VertexBuffer()
{
	AmberGL::DeleteBuffers(1, &m_bufferID);
}

void OnyxEditor::Buffers::VertexBuffer::Bind() const
{
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, m_bufferID);
}

void OnyxEditor::Buffers::VertexBuffer::Unbind() const
{
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, 0);
}

uint32_t OnyxEditor::Buffers::VertexBuffer::GetID()
{
	return m_bufferID;
}
