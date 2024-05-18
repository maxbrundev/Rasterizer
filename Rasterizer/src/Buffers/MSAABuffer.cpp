#include "Buffers/MSAABuffer.h"

Buffers::MSAABuffer::MSAABuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_samplesCount(0),
Data(nullptr)
{
}

Buffers::MSAABuffer::~MSAABuffer()
{
	if(Data != nullptr)
	{
		delete[] Data;
		Data = nullptr;
	}
}

void Buffers::MSAABuffer::SetSamplesAmount(uint8_t p_amount)
{
	m_samplesCount = p_amount;

	if(Data != nullptr)
	{
		delete[] Data;
		Data = nullptr;
	}

	Data = new Sample[m_width * m_height * m_samplesCount];
}

void Buffers::MSAABuffer::Clear(const Data::Color& p_color) const
{
	if(m_samplesCount == 0)
		return;

	const uint32_t color = p_color.Pack();

	const uint32_t samplesCount = m_width * m_height * m_samplesCount;

	for (uint32_t i = 0; i < samplesCount; i++)
	{
		Data[i].Color = color;
		Data[i].Depth = std::numeric_limits<float>::max();
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

size_t Buffers::MSAABuffer::GetIndex(uint32_t x, uint32_t y, uint8_t sampleIndex) const
{
	return (y * m_width + x) * m_samplesCount + sampleIndex;
}

Buffers::Sample& Buffers::MSAABuffer::GetSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex) const
{
	return Data[GetIndex(p_x, p_y, p_sampleIndex)];
}

void Buffers::MSAABuffer::SetPixelSample(uint32_t x, uint32_t y, uint8_t sampleIndex, const Data::Color& color, float depth) const
{
	auto& sample = Data[GetIndex(x, y, sampleIndex)];

	if (depth < sample.Depth)
	{
		sample.Color = color.Pack();
		sample.Depth = depth;
	}
}
