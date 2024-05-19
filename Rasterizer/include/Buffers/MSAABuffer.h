#pragma once

#include "Rendering/Sample.h"
#include "Data/Color.h"

namespace Buffers
{
	class MSAABuffer
	{
	public:
		MSAABuffer(uint32_t p_width, uint32_t p_height);
		~MSAABuffer();

		void SetSamplesAmount(uint8_t p_amount);

		void Clear(const Data::Color& p_color) const;

		void SetPixelSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex, const Data::Color& color, float depth) const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		Rendering::Sample& GetSample(uint32_t p_x, uint32_t p_y, uint8_t p_sampleIndex) const;

	private:
		uint32_t m_width;
		uint32_t m_height;
		uint8_t m_samplesAmount;

		Rendering::Sample* m_data;
	};
}