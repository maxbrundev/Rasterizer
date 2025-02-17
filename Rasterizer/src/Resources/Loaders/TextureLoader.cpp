#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Resources/Loaders/TextureLoader.h"

#include <iostream>

std::string Resources::Loaders::TextureLoader::FILE_TRACE;

Resources::Texture* Resources::Loaders::TextureLoader::Create(std::string p_filePath, bool p_flipVertically, Settings::ETextureFilteringMode p_filter, Settings::ETextureWrapMode p_wrapping, bool p_generateMipmap)
{
	FILE_TRACE = p_filePath;

	int textureWidth;
	int textureHeight;
	int bitsPerPixel;

	stbi_set_flip_vertically_on_load(p_flipVertically);

	if (unsigned char* dataBuffer = stbi_load(p_filePath.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4); dataBuffer)
	{
		Texture* texture = new Texture(std::move(p_filePath), textureWidth, textureHeight, bitsPerPixel, dataBuffer, p_filter, p_wrapping, p_generateMipmap);

		return texture;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << FILE_TRACE << '\n';

		stbi_image_free(dataBuffer);
	}

	return nullptr;
}

bool Resources::Loaders::TextureLoader::Destroy(Texture*& p_textureInstance)
{
	if (p_textureInstance)
	{
		delete p_textureInstance;
		p_textureInstance = nullptr;

		return true;
	}

	return false;
}

Resources::Texture* Resources::Loaders::TextureLoader::CreateColor(uint32_t color, Settings::ETextureFilteringMode p_filter, Settings::ETextureWrapMode p_wrapping)
{
	uint32_t width = 1;
	uint32_t height = 1;
	uint32_t bitsPerPixel = 32;

	unsigned char data[4];

	data[0] = (color >> 24) & 0xFF;
	data[1] = (color >> 16) & 0xFF;
	data[2] = (color >> 8) & 0xFF;
	data[3] = color & 0xFF;

	unsigned char* textureData = new unsigned char[4];
	std::memcpy(textureData, data, 4);

	return new Texture("color", width, height, bitsPerPixel, textureData, p_filter, p_wrapping, false);
}
