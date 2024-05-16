#include "Resources/Texture.h"

#include <algorithm>
#include <cmath>

Resources::Texture::Texture(std::string p_filePath, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, unsigned char* p_data, ETextureFilteringMode p_filter, ETextureWrapMode p_wrapping, bool p_generateMipmap) :
Path(std::move(p_filePath)),
Width(p_width),
Height(p_width),
BitsPerPixel(p_bitsPerPixel),
Filter(p_filter),
Wrapping(p_wrapping),
HasMipmaps(p_generateMipmap)
{
	Data = new uint8_t[Width * Height * 4];
	std::memcpy(Data, p_data, Width * Height * 4);

	if(p_generateMipmap)
	{
		Mipmaps.emplace_back(new uint8_t[Width * Height * 4], Width, Height);
		std::memcpy(Mipmaps[0].Data, Data, Width * Height * 4);

		GenerateMipmaps();
	}
}

Resources::Texture::~Texture()
{
	delete[] Data;
	Data = nullptr;

	for (auto& mipmap : Mipmaps)
	{
		delete[] mipmap.Data;
		mipmap.Data = nullptr;
	}
}

void Resources::Texture::GenerateMipmaps()
{
	int levelWidth  = Width;
	int levelHeight = Height;

	int maxLevel = 1 + floor(std::log2(std::max(Width, Height)));

	for (int i = 0; i < maxLevel; i++)
	{
		levelWidth = std::max(1, levelWidth / 2);
		levelHeight = std::max(1, levelHeight / 2);

		unsigned char* newData = new unsigned char[levelWidth * levelHeight * 4];

		auto& previousData = Mipmaps[i];

		float scaleX = static_cast<float>(previousData.Width) / levelWidth;
		float scaleY = static_cast<float>(previousData.Height) / levelHeight;

		for (int y = 0; y < levelHeight; y++)
		{
			for (int x = 0; x < levelWidth; x++)
			{
				float texelX = x * scaleX;
				float texelY = y * scaleY;

				int x0 = static_cast<int>(std::floor(texelX));
				int y0 = static_cast<int>(std::floor(texelY));

				int x1 = std::min(x0 + 1, static_cast<int>(previousData.Width) - 1);
				int y1 = std::min(y0 + 1, static_cast<int>(previousData.Height) - 1);

				float tx = texelX - x0;
				float ty = texelY - y0;

				for (int j = 0; j < 4; j++)
				{
					float value00 = previousData.Data[(y0 * previousData.Width + x0) * 4 + j] / 255.0f;
					float value01 = previousData.Data[(y1 * previousData.Width + x0) * 4 + j] / 255.0f;
					float value10 = previousData.Data[(y0 * previousData.Width + x1) * 4 + j] / 255.0f;
					float value11 = previousData.Data[(y1 * previousData.Width + x1) * 4 + j] / 255.0f;

					float interpolatedValue = (1.0f - tx) * (1.0f - ty) * value00 +
						tx * (1.0f - ty) * value10 +
						(1 - tx) * ty * value01 +
						tx * ty * value11;

					interpolatedValue = std::max(0.0f, std::min(1.0f, interpolatedValue));

					newData[(y * levelWidth + x) * 4 + j] = static_cast<unsigned char>(interpolatedValue * 255.0f);
				}
			}
		}

		Mipmaps.emplace_back(newData, levelWidth, levelHeight);
	}
}
