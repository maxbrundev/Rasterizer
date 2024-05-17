#pragma once

#include <string>

#include "Resources/Texture.h"

namespace Resources::Loaders
{
	class TextureLoader
	{
	public:
		TextureLoader() = delete;

		static Texture* Create(std::string p_filePath, bool p_flipVertically, Settings::ETextureFilteringMode p_filter, Settings::ETextureWrapMode p_wrapping, bool p_generateMipmap);
		static bool Destroy(Texture*& p_textureInstance);

	private:
		static std::string FILE_TRACE;
	};
}
