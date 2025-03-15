#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "AmberRenderer/Resources/Loaders/TextureLoader.h"

#include <iostream>

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"
#include "AmberRenderer/Tools/Utils/String.h"

std::string AmberRenderer::Resources::Loaders::TextureLoader::FILE_TRACE;

AmberRenderer::Resources::Texture* AmberRenderer::Resources::Loaders::TextureLoader::Create(std::string p_filePath, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter, Settings::ETextureWrapMode p_wrapS, Settings::ETextureWrapMode p_wrapT, bool p_flipVertically, bool p_generateMipmap)
{
	FILE_TRACE = p_filePath;

	uint32_t textureID;
	int textureWidth;
	int textureHeight;
	int bitsPerPixel;

	GLRasterizer::GenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(p_flipVertically);

	if (unsigned char* dataBuffer = stbi_load(p_filePath.c_str(), &textureWidth, &textureHeight, &bitsPerPixel, 4); dataBuffer)
	{
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, textureID);

		GLRasterizer::TexImage2D(GLR_TEXTURE_2D, 0, GLR_RGBA8, textureWidth, textureHeight, 0, GLR_RGBA8 /*GLR_RGBA*/, GLR_UNSIGNED_BYTE, dataBuffer);

		if (p_generateMipmap)
		{
			GLRasterizer::GenerateMipmap(GLR_TEXTURE_2D);
		}

		GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_S, p_wrapS);
		GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_T, p_wrapT);
		GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MIN_FILTER, p_minFilter);
		GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MAG_FILTER, p_magFilter);

		stbi_image_free(dataBuffer);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

		std::string name = Tools::Utils::String::ExtractFileNameFromPath(p_filePath);

		return new Texture(p_filePath, textureID, textureWidth, textureHeight, bitsPerPixel, p_minFilter, p_magFilter, p_wrapS, p_wrapT, p_generateMipmap);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << FILE_TRACE << std::endl;

		stbi_image_free(dataBuffer);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);
	}

	return nullptr;
}

AmberRenderer::Resources::Texture* AmberRenderer::Resources::Loaders::TextureLoader::CreateColor(uint32_t p_data, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter)
{
	uint32_t textureID;
	GLRasterizer::GenTextures(1, &textureID);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, textureID);

	GLRasterizer::TexImage2D(GLR_TEXTURE_2D, 0, GLR_RGBA8, 1, 1, 0, GLR_RGBA8 /*GLR_RGBA*/, GLR_UNSIGNED_BYTE, &p_data);

	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_S, GLR_REPEAT);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_T, GLR_REPEAT);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MIN_FILTER, p_minFilter);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MAG_FILTER, p_magFilter);

	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

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
		GLRasterizer::DeleteTextures(1, &p_textureInstance->ID);

		return true;
	}

	return false;
}
