#include "OnyxEditor/Buffers/UniformBuffer.h"

#include "OnyxEditor/Buffers/FrameBuffer.h"

OnyxEditor::Buffers::UniformBuffer::UniformBuffer(size_t p_size, std::uint32_t p_bindingPoint) : m_bindingPoint(p_bindingPoint), m_size(p_size)
{
    AmberGL::GenUniformBuffers(1, &m_bufferID);
    AmberGL::BindUniformBuffer(m_bufferID);
    AmberGL::UniformBufferData(p_size, nullptr, 0);
    AmberGL::BindUniformBufferBase(p_bindingPoint, m_bufferID);
}

OnyxEditor::Buffers::UniformBuffer::~UniformBuffer()
{
    AmberGL::DeleteUniformBuffers(1, &m_bufferID);
}

void OnyxEditor::Buffers::UniformBuffer::Bind() const
{
    AmberGL::BindUniformBuffer(m_bufferID);
}

void OnyxEditor::Buffers::UniformBuffer::Unbind() const
{
    AmberGL::BindUniformBuffer(0);
}

void OnyxEditor::Buffers::UniformBuffer::SetSubData(const void* p_data, size_t p_dataSize, size_t p_offset)
{
    AmberGL::BindUniformBuffer(m_bufferID);
    AmberGL::UniformBufferSubData(p_offset, p_dataSize, p_data);
}
