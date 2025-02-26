#pragma once

#include <algorithm>
#include <cstdint>

#include "Data/Color.h"

typedef Data::Color RGBA8;
typedef float Depth;

namespace Rendering::Rasterizer::Buffers
{
	static uint32_t floatToUint32(float p_value)
	{
		uint32_t bits;
		std::memcpy(&bits, &p_value, sizeof(float));
		return bits;
	}

	static float uint32ToFloat(uint32_t p_bits)
	{
		float value;
		std::memcpy(&value, &p_bits, sizeof(float));
		return value;
	}

	//TODO: support multiple types <RGBA8, Depth, vec4, vec3>
	template<typename T>
	class FrameBuffer
	{
		static_assert(std::is_same_v<T, RGBA8> || std::is_same_v<T, Depth>, "FrameBuffer only supports RGBA8 (Color) or Depth (float).");

	public:
		FrameBuffer(uint32_t p_width, uint32_t p_height) :
		m_width(p_width),
		m_height(p_height),
		m_size(m_width* m_height),
		m_rowSize(m_width * sizeof(uint32_t)),
		m_sizeInByte(m_size * sizeof(uint32_t)),
		m_data(new uint32_t[m_size])
		{
		}

		~FrameBuffer()
		{
			delete[] m_data;
			m_data = nullptr;
		}

		void Clear(const Data::Color& p_color) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				std::fill(m_data, m_data + m_size - 1, p_color.Pack());
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				uint32_t value = floatToUint32(std::numeric_limits<float>::max());
				std::fill(m_data, m_data + m_size - 1, value);
			}
		}

		void SetPixel(int p_x, int p_y, const T& p_value) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				m_data[p_y * m_width + p_x] = p_value.Pack();
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				m_data[p_y * m_width + p_x] = floatToUint32(p_value);;
			}
		}

		T GetPixel(uint32_t p_x, uint32_t p_y) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				return m_data[p_y * m_width + p_x];
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				return uint32ToFloat(m_data[p_y * m_width + p_x]);
			}

			return T{};
		}

		void Resize(uint32_t p_width, uint32_t p_height)
		{
			uint32_t* newArray = new uint32_t[p_width * p_height];

			for (size_t i = 0; i < std::min(m_size, p_width * p_height); i++)
			{
				newArray[i] = m_data[i];
			}

			delete[] m_data;

			m_data = newArray;

			m_width = p_width;
			m_height = p_height;
			m_size = m_width * m_height;
			m_rowSize = m_width * sizeof(uint32_t);
			m_sizeInByte = m_size * sizeof(uint32_t);

			Clear({ 0, 0, 0 });
		}

		uint32_t GetWidth() const
		{
			return m_width;
		}

		uint32_t GetHeight() const
		{
			return m_height;
		}

		uint32_t* GetData() const
		{
			return m_data;
		}

		uint32_t GetRawSize() const
		{
			return m_rowSize;
		}

	private:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_rowSize;
		uint32_t m_sizeInByte;

		uint32_t* m_data;
	};
}
