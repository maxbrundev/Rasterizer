#pragma once
#include <cstdint>

namespace Buffers
{
	class DepthBuffer
	{
	public:
		DepthBuffer(uint32_t p_width, uint32_t p_height);
		~DepthBuffer();

		float GetElement(uint32_t p_x, uint32_t p_y) const;
		void SetElement(uint32_t p_x, uint32_t p_y, const float& p_value) const;

		void Clear() const;

	private:
		float* data;

		const uint32_t m_width;
		const uint32_t m_height;
		const uint32_t m_bufferSize;
		const uint32_t m_bufferSizeInBytes;
	};
}