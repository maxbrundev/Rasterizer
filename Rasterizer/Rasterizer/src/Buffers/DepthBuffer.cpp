#include "Buffers/DepthBuffer.h"

#include <algorithm>
#include <limits>

Buffers::DepthBuffer::DepthBuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_size(m_width * m_height),
m_sizeInByte(m_size * sizeof(float)),
m_data(new float[m_size])
{
	
}

Buffers::DepthBuffer::~DepthBuffer()
{
	delete[] m_data;
	m_data = nullptr;
}

float Buffers::DepthBuffer::GetElement(uint32_t p_x, uint32_t p_y) const
{
	return m_data[p_y * m_width + p_x];
}

void Buffers::DepthBuffer::SetElement(uint32_t p_x, uint32_t p_y, const float& p_value) const
{
	m_data[p_y * m_width + p_x] = p_value;
}

void Buffers::DepthBuffer::Clear() const
{
	for (uint32_t i = 0; i < m_size; ++i)
	{
		m_data[i] = std::numeric_limits<float>::max();
	}
}

void Buffers::DepthBuffer::Resize(uint32_t p_width, uint32_t p_height)
{
	float* newArray = new float[p_width * p_height];

	for (size_t i = 0; i < std::min(m_size, p_width * p_height); i++)
	{
		newArray[i] = m_data[i];
	}

	delete[] m_data;

	m_data = newArray;

	m_width = p_width;
	m_height = p_height;
	m_size = m_width * m_height;
	m_sizeInByte = m_size * sizeof(uint32_t);

	Clear();
}

uint32_t Buffers::DepthBuffer::GetWidth() const
{
	return m_width;
}

uint32_t Buffers::DepthBuffer::GetHeight() const
{
	return m_height;
}
