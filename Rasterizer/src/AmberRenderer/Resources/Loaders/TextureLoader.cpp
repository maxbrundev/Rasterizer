#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "AmberRenderer/Resources/Loaders/TextureLoader.h"

#include <iostream>

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"
#include "AmberRenderer/Tools/Utils/String.h"

std::string AmberRenderer::Resources::Loaders::TextureLoader::FILE_TRACE;

AmberRenderer::Resources::Texture* AmberRenderer::Resources::Loaders::TextureLoader::Create(std::string p_filePath, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter, Settings::ETextureWrapMode p_wrapS, Settings::ETextureWrapMode p_wrapT, bool p_flipVertically, bool p_generateMipmap)
{
	FILE_TRACE = p_filePath;

	uint32_t textureID;
	int textureWidth;
	int textureHeight;
	int bitsPerPixel;

	AmberGL::GenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(p_flipVertically);

	if (unsigned char* dataBuffer = stbi_load(p_filePath.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4); dataBuffer)
	{
		AmberGL::BindTexture(AGL_TEXTURE_2D, textureID);

		AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, textureWidth, textureHeight, 0, AGL_RGBA8 /*AGL_RGBA*/, AGL_UNSIGNED_BYTE, dataBuffer);

		if (p_generateMipmap)
		{
			AmberGL::GenerateMipmap(AGL_TEXTURE_2D);
		}

		AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, p_wrapS);
		AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, p_wrapT);
		AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, p_minFilter);
		AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, p_magFilter);

		stbi_image_free(dataBuffer);
		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

		std::string name = Tools::Utils::String::ExtractFileNameFromPath(p_filePath);

		return new Texture(p_filePath, textureID, textureWidth, textureHeight, bitsPerPixel, p_minFilter, p_magFilter, p_wrapS, p_wrapT, p_generateMipmap);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << FILE_TRACE << std::endl;

		stbi_image_free(dataBuffer);
		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);
	}

	return nullptr;
}

AmberRenderer::Resources::Texture* AmberRenderer::Resources::Loaders::TextureLoader::CreateColor(uint32_t p_data, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter)
{
	uint32_t textureID;
	AmberGL::GenTextures(1, &textureID);
	AmberGL::BindTexture(AGL_TEXTURE_2D, textureID);

	AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, 1, 1, 0, AGL_RGBA8 /*AGL_RGBA*/, AGL_UNSIGNED_BYTE, &p_data);

	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, AGL_REPEAT);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, AGL_REPEAT);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, p_minFilter);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, p_magFilter);

	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	return new Texture("", textureID, 1, 1, 32, p_minFilter, p_magFilter, Settings::REPEAT, Settings::REPEAT, false);
}

bool AmberRenderer::Resources::Loaders::TextureLoader::Destroy(Texture*& p_textureInstance)
{
	if (p_textureInstance)
	{
		delete p_textureInstance;
		p_textureInstance = nullptr;

		return true;
	}

	return false;
}

bool AmberRenderer::Resources::Loaders::TextureLoader::Delete(Texture* p_textureInstance)
{
	if (p_textureInstance)
	{
		AmberGL::DeleteTextures(1, &p_textureInstance->ID);

		return true;
	}

	return false;
}
