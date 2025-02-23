#pragma once

#include <cstdint>

#include "Data/Color.h"

namespace Rendering::Rasterizer::Buffers
{
	class TextureBuffer
	{
	public:
		TextureBuffer(uint32_t p_width, uint32_t p_height);
		~TextureBuffer();

		void Clear(const Data::Color& p_color) const;

		void SetPixel(int p_x, int p_y, const Data::Color& p_color) const;
		uint32_t GetPixel(uint32_t p_x, uint32_t p_y) const;

		void Resize(uint32_t p_width, uint32_t p_height);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		uint32_t* GetData();
		uint32_t GetRawSize();

	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_rowSize;
		uint32_t m_sizeInByte;

		uint32_t* m_data;
	};
}
