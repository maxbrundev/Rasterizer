#include "Rendering/Rasterizer/Buffers/TextureBuffer.h"

#include <algorithm>

Rendering::Rasterizer::Buffers::TextureBuffer::TextureBuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_size(m_width * m_height),
m_rowSize(m_width * sizeof(uint32_t)),
m_sizeInByte(m_size * sizeof(uint32_t)),
m_data(new uint32_t[m_size])
{
}

Rendering::Rasterizer::Buffers::TextureBuffer::~TextureBuffer()
{
	delete[] m_data;
	m_data = nullptr;
}

void Rendering::Rasterizer::Buffers::TextureBuffer::Clear(const Data::Color& p_color) const
{
	const uint32_t color = p_color.Pack();
	std::fill(m_data, m_data + m_size - 1, color);
}

void Rendering::Rasterizer::Buffers::TextureBuffer::SetPixel(int p_x, int p_y, const Data::Color& p_color) const
{
	m_data[p_y * m_width + p_x] = p_color.Pack();
}

uint32_t Rendering::Rasterizer::Buffers::TextureBuffer::GetPixel(uint32_t p_x, uint32_t p_y) const
{
	return m_data[p_y * m_width + p_x];
}

void Rendering::Rasterizer::Buffers::TextureBuffer::Resize(uint32_t p_width, uint32_t p_height)
{
	uint32_t* newArray = new uint32_t[p_width * p_height];

	for (size_t i = 0; i < std::min(m_size, p_width * p_height); i++)
	{
		newArray[i] = m_data[i];
	}

	delete[] m_data;

	m_data = newArray;

	m_width      = p_width;
	m_height     = p_height;
	m_size       = m_width * m_height;
	m_rowSize    = m_width * sizeof(uint32_t);
	m_sizeInByte = m_size * sizeof(uint32_t);

	Clear({ 0, 0, 0 });
}

uint32_t Rendering::Rasterizer::Buffers::TextureBuffer::GetWidth() const
{
	return m_width;
}

uint32_t Rendering::Rasterizer::Buffers::TextureBuffer::GetHeight() const
{
	return m_height;
}

uint32_t* Rendering::Rasterizer::Buffers::TextureBuffer::GetData()
{
	return m_data;
}

uint32_t Rendering::Rasterizer::Buffers::TextureBuffer::GetRawSize()
{
	return m_rowSize;
}
