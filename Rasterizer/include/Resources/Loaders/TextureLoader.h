#pragma once

#include <string>

#include "Resources/Texture.h"

namespace Resources::Loaders
{
	class TextureLoader
	{
	public:
		TextureLoader() = delete;

		static Texture* Create(std::string p_filePath, bool p_flipVertically, ETextureFilteringMode p_filter, ETextureWrapMode p_wrapping, bool p_generateMipmap);
		static bool Destroy(Texture*& p_textureInstance);

	private:
		static std::string FILE_TRACE;
	};
}