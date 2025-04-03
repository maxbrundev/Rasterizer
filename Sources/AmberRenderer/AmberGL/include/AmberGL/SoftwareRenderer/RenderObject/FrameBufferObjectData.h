#pragma once

#include <algorithm>
#include <cstdint>
#include <emmintrin.h>

#include "AmberGL/SoftwareRenderer/RenderObject/TextureObject.h"

typedef uint32_t RGBA8;
typedef float Depth;

inline void SSEClear(uint32_t* p_data, size_t p_size, uint32_t p_value)
{
	__m128i fillValue = _mm_set1_epi32(static_cast<int>(p_value));

	size_t blockCount = p_size / 4;

	for (size_t i = 0; i < blockCount; ++i)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(p_data + i * 4), fillValue);
	}

	for (size_t i = blockCount * 4; i < p_size; ++i)
	{
		p_data[i] = p_value;
	}
}

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

namespace AmberGL::SoftwareRenderer::RenderObject
{
	template<typename T>
	struct FrameBufferObjectData
	{
		static_assert(std::is_same_v<T, RGBA8> || std::is_same_v<T, Depth>, "FrameBuffer only supports RGBA8 (Color) or Depth (float).");

		uint32_t Width;
		uint32_t Height;
		uint32_t Size;
		uint32_t RowSize;
		uint32_t SizeInByte;
		uint32_t ClearColor;
		uint32_t* Data;

		FrameBufferObjectData(uint32_t p_width, uint32_t p_height) :
			Width(p_width),
			Height(p_height),
			Size(Width* Height),
			RowSize(Width * sizeof(uint32_t)),
			SizeInByte(Size * sizeof(uint32_t)),
			ClearColor(0),
			Data(new uint32_t[Size])
		{
		}

		~FrameBufferObjectData()
		{
			delete[] Data;
			Data = nullptr;
		}

		void SetColor(float p_red, float p_green, float p_blue, float p_alpha)
		{
			uint8_t r = static_cast<uint8_t>(std::round(std::clamp(p_red, 0.0f, 1.0f) * 255.0f));
			uint8_t g = static_cast<uint8_t>(std::round(std::clamp(p_green, 0.0f, 1.0f) * 255.0f));
			uint8_t b = static_cast<uint8_t>(std::round(std::clamp(p_blue, 0.0f, 1.0f) * 255.0f));
			uint8_t a = static_cast<uint8_t>(std::round(std::clamp(p_alpha, 0.0f, 1.0f) * 255.0f));

			ClearColor = (r << 24) | (g << 16) | (b << 8) | a;
		}

		void Clear() const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				SSEClear(Data, Size, ClearColor);
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				uint32_t depthVal = floatToUint32(std::numeric_limits<float>::max());
				SSEClear(Data, Size, depthVal);
			}
		}

		void Resize(uint32_t p_width, uint32_t p_height)
		{
			if (Width == p_width && Height == p_height)
				return;

			delete[] Data;

			Width = p_width;
			Height = p_height;
			Size = Width * Height;
			RowSize = Width * sizeof(uint32_t);

			Data = new uint32_t[Size];
			Clear();
		}

		void SetPixel(int p_x, int p_y, const T& p_value) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				Data[p_y * Width + p_x] = p_value;
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				Data[p_y * Width + p_x] = floatToUint32(p_value);
			}
		}

		T GetPixel(uint32_t p_x, uint32_t p_y) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				return Data[p_y * Width + p_x];
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				return uint32ToFloat(Data[p_y * Width + p_x]);
			}

			return T{};
		}

		void UpdateAttachedTextureObject(TextureObject* p_textureObject) const
		{
			if (!p_textureObject)
				return;

			uint32_t bytes = Size * 4;
			uint8_t* textureData = new uint8_t[bytes];

			if constexpr (std::is_same<T, Depth>::value)
			{
				for (uint32_t y = 0; y < Height; y++)
				{
					for (uint32_t x = 0; x < Width; x++)
					{
						uint32_t flippedY = Height - 1 - y;
						uint32_t index = flippedY * Width + x;

						float depth = uint32ToFloat(Data[y * Width + x]);
						depth = std::clamp(depth, 0.0f, 1.0f);
						uint8_t d = static_cast<uint8_t>(depth * 255.0f);

						textureData[index * 4 + 0] = d;
						textureData[index * 4 + 1] = 0;
						textureData[index * 4 + 2] = 0;
						textureData[index * 4 + 3] = 255;
					}
				}
			}
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				for (uint32_t y = 0; y < Height; y++)
				{
					for (uint32_t x = 0; x < Width; x++)
					{
						uint32_t flippedY = Height - 1 - y;
						uint32_t index = flippedY * Width + x;

						uint32_t color = Data[y * Width + x];

						textureData[index * 4 + 0] = color >> 24;
						textureData[index * 4 + 1] = color >> 16;
						textureData[index * 4 + 2] = color >> 8;
						textureData[index * 4 + 3] = color;
					}
				}
			}

			std::memcpy(p_textureObject->Data8, textureData, bytes);
			delete[] textureData;
		}
	};
}
