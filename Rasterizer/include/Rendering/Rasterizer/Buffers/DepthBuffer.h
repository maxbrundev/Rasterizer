#pragma once

#include <cstdint>

namespace Rendering::Rasterizer::Buffers
{
	class DepthBuffer
	{
	public:
		DepthBuffer(uint32_t p_width, uint32_t p_height);
		~DepthBuffer();

		float GetElement(uint32_t p_x, uint32_t p_y) const;
		void SetElement(uint32_t p_x, uint32_t p_y, const float& p_value) const;

		void Clear() const;

		void Resize(uint32_t p_width, uint32_t p_height);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_sizeInByte;

		float* m_data;
	};
}