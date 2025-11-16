#include "AmberGL/SoftwareRenderer/Buffers/MSAABuffer.h"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>


static inline RGBA8 pack(const glm::vec4& c)
{
	return (uint8_t)(c.r * 255.f) << 24 |
		(uint8_t)(c.g * 255.f) << 16 |
		(uint8_t)(c.b * 255.f) << 8 |
		(uint8_t)(c.a * 255.f);
}
static inline glm::vec4 unpack(RGBA8 p)
{
	return {
		((p >> 24) & 0xFF) / 255.f,
		((p >> 16) & 0xFF) / 255.f,
		((p >> 8) & 0xFF) / 255.f,
		(p & 0xFF) / 255.f
	};
}

AmberGL::SoftwareRenderer::Buffers::MSAABuffer::MSAABuffer(uint32_t p_width, uint32_t p_height) :
m_width(p_width),
m_height(p_height),
m_samplesAmount(0),
m_data(nullptr),
m_clearColor(0)
{
	SetSamplesAmount(4);
}

AmberGL::SoftwareRenderer::Buffers::MSAABuffer::~MSAABuffer()
{
	if(m_data != nullptr)
	{
		delete[] m_data;
		m_data = nullptr;
	}
}

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::BindFrameBuffers(RenderObject::FrameBufferObjectData<RGBA8>* p_frameBufferColor, RenderObject::FrameBufferObjectData<Depth>* p_frameBufferDepth)
{
	m_frameBufferColor = p_frameBufferColor;
	m_frameBufferDepth = p_frameBufferDepth;
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

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::Resize(uint32_t p_width, uint32_t p_height)
{
	if (m_width == p_width && m_height == p_height)
		return;

	delete[] m_data;

	m_width = p_width;
	m_height = p_height;

	m_data = new Sample[m_width * m_height * m_samplesAmount];

	Clear();
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

void AmberGL::SoftwareRenderer::Buffers::MSAABuffer::SetPixelSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex, uint32_t p_color, float p_depth, bool p_isDepthTesting) const
{
	Sample& sample = m_data[(p_y * m_width + p_x) * m_samplesAmount + p_sampleIndex];

	if (!p_isDepthTesting || p_depth < sample.Depth)
	{
		sample.Color = p_color; sample.Depth = p_depth;
	}

	if (!m_frameBufferColor && !m_frameBufferDepth) 
		return;

	glm::ivec4 sum(0);
	float depthSum = 0.0f;

	const size_t base = (p_y * m_width + p_x) * m_samplesAmount;

	for (uint8_t i = 0; i < m_samplesAmount; ++i)
	{
		const Sample& currentSample = m_data[base + i];
		sum.r += (currentSample.Color >> 24) & 0xFF;
		sum.g += (currentSample.Color >> 16) & 0xFF;
		sum.b += (currentSample.Color >> 8) & 0xFF;
		sum.a += currentSample.Color & 0xFF;
		depthSum += currentSample.Depth;
	}

	const float invN = 1.0f / static_cast<float>(m_samplesAmount);

	if (m_frameBufferColor)
	{
		glm::vec4 avg = {sum.r * invN / 255.0f, sum.g * invN / 255.0f, sum.b * invN / 255.0f, sum.a * invN / 255.0f};
		glm::vec4 dst = unpack(m_frameBufferColor->GetPixel(p_x, p_y));
		glm::vec4 out = glm::mix(dst, avg, avg.a);
		m_frameBufferColor->SetPixel(p_x, p_y, pack(out));
	}

	if (m_frameBufferDepth)
	{
		m_frameBufferDepth->SetPixel(p_x, p_y, depthSum * invN);
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