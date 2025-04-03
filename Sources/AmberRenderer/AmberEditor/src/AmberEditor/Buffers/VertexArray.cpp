#include "AmberEditor/Buffers/VertexArray.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

AmberEditor::Buffers::VertexArray::VertexArray()
{
	AmberGL::GenVertexArrays(1, &m_bufferID);
	Bind();
}

AmberEditor::Buffers::VertexArray::~VertexArray()
{
	AmberGL::DeleteVertexArrays(1, &m_bufferID);
}

void AmberEditor::Buffers::VertexArray::Bind() const
{
	AmberGL::BindVertexArray(m_bufferID);
}

void AmberEditor::Buffers::VertexArray::Unbind() const
{
	AmberGL::BindVertexArray(0);
}

uint32_t AmberEditor::Buffers::VertexArray::GetID() const
{
	return m_bufferID;
}
