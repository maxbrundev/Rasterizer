#include "OnyxEditor/Buffers/VertexArray.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

OnyxEditor::Buffers::VertexArray::VertexArray()
{
	AmberGL::GenVertexArrays(1, &m_bufferID);
	Bind();
}

OnyxEditor::Buffers::VertexArray::~VertexArray()
{
	AmberGL::DeleteVertexArrays(1, &m_bufferID);
}

void OnyxEditor::Buffers::VertexArray::Bind() const
{
	AmberGL::BindVertexArray(m_bufferID);
}

void OnyxEditor::Buffers::VertexArray::Unbind() const
{
	AmberGL::BindVertexArray(0);
}

uint32_t OnyxEditor::Buffers::VertexArray::GetID() const
{
	return m_bufferID;
}
