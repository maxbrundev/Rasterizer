#pragma once

#include <vector>

#include "Data/Color.h"

namespace Buffers
{
	class MSAABuffer
	{
	public:
		MSAABuffer(uint32_t p_width, uint32_t p_height);
		~MSAABuffer();

		void SetSamplesAmount(uint8_t p_amount);

		void Clear(const Data::Color& p_color);

		void SetPixelSample(int x, int y, int sampleIndex, const Data::Color& color, float depth);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		
	private:
		uint32_t m_width;
		uint32_t m_height;
		
	public:
		std::vector<std::vector<std::pair<uint32_t, float>>> Data;
	};
}