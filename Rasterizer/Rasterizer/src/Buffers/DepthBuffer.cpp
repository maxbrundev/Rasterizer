#include "Buffers/DepthBuffer.h"

#include <limits>

Buffers::DepthBuffer::DepthBuffer(uint32_t p_width, uint32_t p_height) : m_width(p_width), m_height(p_height), m_bufferSize(m_width * m_height), m_bufferSizeInBytes(static_cast<uint32_t>(m_bufferSize * sizeof(float)))
{
	data = new float[m_bufferSize];
}

Buffers::DepthBuffer::~DepthBuffer()
{
	delete[] data;
	data = nullptr;
}

float Buffers::DepthBuffer::GetElement(uint32_t p_x, uint32_t p_y) const
{
	return data[p_y * m_width + p_x];
}

void Buffers::DepthBuffer::SetElement(uint32_t p_x, uint32_t p_y, const float& p_value) const
{
	data[p_y * m_width + p_x] = p_value;
}

void Buffers::DepthBuffer::Clear() const
{
	for (uint32_t i = 0; i < m_bufferSize; ++i)
	{
		data[i] = std::numeric_limits<float>::infinity();
	}
}
