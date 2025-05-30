#include "AmberGL/SoftwareRenderer/Buffers/MSAABuffer.h"

#include <algorithm>
#include <cmath>

AmberGL::SoftwareRenderer::Buffers::MSAABuffer::MSAABuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_samplesAmount(0),
m_data(nullptr),
m_clearColor(0)
{
}

AmberGL::SoftwareRenderer::Buffers::MSAABuffer::~MSAABuffer()
{
	if(m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::SetSamplesAmount(uint8_t p_amount)
{
	m_samplesAmount = p_amount;

	if(m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}

	m_data = new Sample[m_width * m_height * m_samplesAmount];
}

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::Clear() const
{
	if(m_samplesAmount == 0)
		return;

	const uint32_t samplesCount = m_width * m_height * m_samplesAmount;

	for (uint32_t i = 0; i < samplesCount; i++)
	{
		m_data[i].Color = m_clearColor;
		m_data[i].Depth = std::numeric_limits<float>::max();
	}
}

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::SetColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	uint8_t r = static_cast<uint8_t>(std::round(std::clamp(p_red, 0.0f, 1.0f) * 255.0f));
	uint8_t g = static_cast<uint8_t>(std::round(std::clamp(p_green, 0.0f, 1.0f) * 255.0f));
	uint8_t b = static_cast<uint8_t>(std::round(std::clamp(p_blue, 0.0f, 1.0f) * 255.0f));
	uint8_t a = static_cast<uint8_t>(std::round(std::clamp(p_alpha, 0.0f, 1.0f) * 255.0f));

	m_clearColor = (r << 24) | (g << 16) | (b << 8) | a;
}

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::SetPixelSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex, uint32_t p_color, float p_depth) const
{
	auto& sample = m_data[(p_y * m_width + p_x) * m_samplesAmount + p_sampleIndex];

	if (p_depth < sample.Depth)
	{
		sample.Color = p_color;
		sample.Depth = p_depth;
	}
}

uint32_t AmberGL::SoftwareRenderer::Buffers::MSAABuffer::GetWidth() const
{
	return m_width;
}

uint32_t AmberGL::SoftwareRenderer::Buffers::MSAABuffer::GetHeight() const
{
	return m_height;
}

AmberGL::SoftwareRenderer::Buffers::MSAABuffer::Sample& AmberGL::SoftwareRenderer::Buffers::MSAABuffer::GetSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex) const
{
	return m_data[(p_y * m_width + p_x) * m_samplesAmount + p_sampleIndex];
}