#pragma once

#include <string>

#include "AmberEditor/Resources/Texture.h"

namespace AmberEditor::Resources::Loaders
{
	class TextureLoader
	{
	public:
		TextureLoader() = delete;

		static Texture* Create(std::string p_filePath, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter, Settings::ETextureWrapMode p_wrapS, Settings::ETextureWrapMode p_wrapT, bool p_flipVertically, bool p_generateMipmap);
		static Texture* CreateColor(uint32_t p_data, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter);
		static bool Destroy(Texture*& p_textureInstance);
		static bool Delete(Texture* p_textureInstance);

	private:
		static std::string FILE_TRACE;
	};
}
