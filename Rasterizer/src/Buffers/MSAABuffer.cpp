#include "Buffers/MSAABuffer.h"

Buffers::MSAABuffer::MSAABuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_samplesAmount(0),
m_data(nullptr)
{
}

Buffers::MSAABuffer::~MSAABuffer()
{
	if(m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}

void Buffers::MSAABuffer::SetSamplesAmount(uint8_t p_amount)
{
	m_samplesAmount = p_amount;

	if(m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}

	m_data = new Rendering::Sample[m_width * m_height * m_samplesAmount];
}

void Buffers::MSAABuffer::Clear(const Data::Color& p_color) const
{
	if(m_samplesAmount == 0)
		return;

	const uint32_t color = p_color.Pack();

	const uint32_t samplesCount = m_width * m_height * m_samplesAmount;

	for (uint32_t i = 0; i < samplesCount; i++)
	{
		m_data[i].Color = color;
		m_data[i].Depth = std::numeric_limits<float>::max();
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

Rendering::Sample& Buffers::MSAABuffer::GetSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex) const
{
	return m_data[(p_y * m_width + p_x) * m_samplesAmount + p_sampleIndex];
}

void Buffers::MSAABuffer::SetPixelSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex, const Data::Color& p_color, float p_depth) const
{
	auto& sample = m_data[(p_y * m_width + p_x) * m_samplesAmount + p_sampleIndex];

	if (p_depth < sample.Depth)
	{
		sample.Color = p_color.Pack();
		sample.Depth = p_depth;
	}
}
