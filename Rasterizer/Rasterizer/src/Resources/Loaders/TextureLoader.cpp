#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Resources/Loaders/TextureLoader.h"

#include <iostream>


std::string Resources::Loaders::TextureLoader::FILE_TRACE;

Resources::Texture* Resources::Loaders::TextureLoader::Create(std::string p_filePath, bool p_flipVertically, ETextureFilteringMode p_filter, ETextureWrapMode p_wrapping)
{
	FILE_TRACE = p_filePath;

	int textureWidth;
	int textureHeight;
	int bitsPerPixel;

	stbi_set_flip_vertically_on_load(p_flipVertically);

	if (unsigned char* dataBuffer = stbi_load(p_filePath.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4); dataBuffer)
	{
		Texture* texture = new Texture(std::move(p_filePath), textureWidth, textureHeight, bitsPerPixel, dataBuffer, p_filter, p_wrapping);

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
