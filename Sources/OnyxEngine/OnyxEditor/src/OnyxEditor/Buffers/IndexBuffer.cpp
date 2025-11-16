
#include "OnyxEditor/Buffers/IndexBuffer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

OnyxEditor::Buffers::IndexBuffer::IndexBuffer(const unsigned int* p_data, size_t p_elements)
{
	AmberGL::GenBuffers(1, &m_bufferID);
	Bind();
	AmberGL::BufferData(AGL_ELEMENT_ARRAY_BUFFER, p_elements * sizeof(uint32_t), p_data);
}

OnyxEditor::Buffers::IndexBuffer::~IndexBuffer()
{
	AmberGL::DeleteBuffers(1, &m_bufferID);
}

void OnyxEditor::Buffers::IndexBuffer::Bind() const
{
	AmberGL::BindBuffer(AGL_ELEMENT_ARRAY_BUFFER, m_bufferID);
}

void OnyxEditor::Buffers::IndexBuffer::Unbind() const
{
	AmberGL::BindBuffer(AGL_ELEMENT_ARRAY_BUFFER, m_bufferID);
}

uint32_t OnyxEditor::Buffers::IndexBuffer::GetID() const
{
	return m_bufferID;
}
