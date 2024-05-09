#include "Buffers/MSAABuffer.h"

Buffers::MSAABuffer::MSAABuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height)
{
}

Buffers::MSAABuffer::~MSAABuffer()
{
}

void Buffers::MSAABuffer::SetSamplesAmount(uint8_t p_amount)
{
	Data.clear();
	Data.resize(m_width * m_height, std::vector<std::pair<uint32_t, float>>(p_amount));
}

void Buffers::MSAABuffer::Clear(const Data::Color& p_color)
{
	const uint32_t color = p_color.Pack();

	for (auto& pixel : Data) 
	{
		for (auto& sample : pixel) 
		{
			sample.first = color;
			sample.second = std::numeric_limits<float>::max();
		}
	}
}

uint32_t Buffers::MSAABuffer::GetWidth() const
{
	return m_width;
}

uint32_t Buffers::MSAABuffer::GetHeight() const
{
	return m_height;
}

void Buffers::MSAABuffer::SetPixelSample(int x, int y, int sampleIndex, const Data::Color& color, float depth)
{
	auto& sample = Data[y * m_width + x][sampleIndex];

	if (depth < sample.second)
	{
		sample.first = color.Pack();
		sample.second = depth;
	}
}
