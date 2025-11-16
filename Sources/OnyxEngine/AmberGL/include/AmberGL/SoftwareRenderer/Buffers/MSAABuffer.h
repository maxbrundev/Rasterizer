#pragma once

#include <cstdint>

#include "AmberGL/SoftwareRenderer/RenderObject/FrameBufferObjectData.h"

namespace AmberGL::SoftwareRenderer::Buffers
{
	class MSAABuffer
	{
		struct Sample
		{
			uint32_t Color;
			float Depth;
		};

	public:
		MSAABuffer(uint32_t p_width, uint32_t p_height);
		~MSAABuffer();

		void BindFrameBuffers(RenderObject::FrameBufferObjectData<RGBA8>* p_frameBufferColor, RenderObject::FrameBufferObjectData<Depth>* p_frameBufferDepth);

		void SetSamplesAmount(uint8_t p_amount);
		void Resize(uint32_t p_width, uint32_t p_height);
		void Clear() const;
		void SetColor(float p_red, float p_green, float p_blue, float p_alpha);
		void SetPixelSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex, uint32_t color, float depth, bool p_isDepthTesting) const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		Sample& GetSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex) const;

	private:
		uint32_t m_width;
		uint32_t m_height;
		uint8_t m_samplesAmount;

		Sample* m_data;

		uint32_t m_clearColor;

		RenderObject::FrameBufferObjectData<RGBA8>* m_frameBufferColor;
		RenderObject::FrameBufferObjectData<Depth>* m_frameBufferDepth;
	};
}
