#pragma once

#include <algorithm>
#include <cstdint>
#include <emmintrin.h>

#include "AmberRenderer/Data/Color.h"
#include "AmberRenderer/Resources/Texture.h"

typedef AmberRenderer::Data::Color RGBA8;
typedef float Depth;

namespace AmberRenderer::Rendering::Rasterizer::Buffers
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

	//TODO: encapsulate in utils.
	inline void SSEClear(uint32_t* p_data, size_t p_size, uint32_t p_value)
	{
		__m128i fillValue = _mm_set1_epi32(static_cast<int>(p_value));

		size_t blockCount = p_size / 4;

		for (size_t i = 0; i < blockCount; ++i)
		{
			_mm_storeu_si128(reinterpret_cast<__m128i*>(p_data + i * 4), fillValue);
		}

		size_t start = blockCount * 4;
		for (size_t i = start; i < p_size; ++i)
		{
			p_data[i] = p_value;
		}
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

		void SetColor(float p_red, float p_green, float p_blue, float p_alpha)
		{
			uint8_t r = static_cast<uint8_t>(std::round(std::clamp(p_red, 0.0f, 1.0f) * 255.0f));
			uint8_t g = static_cast<uint8_t>(std::round(std::clamp(p_green, 0.0f, 1.0f) * 255.0f));
			uint8_t b = static_cast<uint8_t>(std::round(std::clamp(p_blue, 0.0f, 1.0f) * 255.0f));
			uint8_t a = static_cast<uint8_t>(std::round(std::clamp(p_alpha, 0.0f, 1.0f) * 255.0f));

			m_clearColor = (r << 24) | (g << 16) | (b << 8) | a;
		}

		void Clear() const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				SSEClear(m_data, m_size, m_clearColor);
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				uint32_t depthVal = floatToUint32(std::numeric_limits<float>::max());
				SSEClear(m_data, m_size, depthVal);
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
			if (m_width == p_width && m_height == p_height)
				return;

			delete[] m_data;

			m_width = p_width;
			m_height = p_height;
			m_size = m_width * m_height;
			m_rowSize = m_width * sizeof(uint32_t);
			m_sizeInByte = m_size * sizeof(uint32_t);

			m_data = new uint32_t[m_size];
			Clear();
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

		void BindTexture(Resources::Texture* p_texture)
		{
			m_texture = p_texture;
		}

		void Bind()
		{
			uint32_t bytes = m_size * 4;
			uint8_t* textureData = new uint8_t[bytes];

			for (uint32_t y = 0; y < m_height; y++)
			{
				for (uint32_t x = 0; x < m_width; x++)
				{
					uint32_t flippedY = m_height - 1 - y;
					uint32_t index = flippedY * m_width + x;

					float depth = uint32ToFloat(m_data[y * m_width + x]);
					depth = std::clamp(depth, 0.0f, 1.0f);
					uint8_t d = static_cast<uint8_t>(depth * 255.0f);
					
					textureData[index * 4 + 0] = d;
					textureData[index * 4 + 1] = 0;
					textureData[index * 4 + 2] = 0;
					textureData[index * 4 + 3] = 255;
				}
			}
			
			std::memcpy(m_texture->Data, textureData, bytes);
			delete[] textureData;
		}

		//TODO: Clean.
		Resources::Texture* m_texture;
	public:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_size;
		uint32_t m_rowSize;
		uint32_t m_sizeInByte;

		uint32_t* m_data;

		uint32_t m_clearColor;
	};
}
