#pragma once

#include <string>

#include "OnyxEditor/Resources/Texture.h"

namespace OnyxEditor::Resources::Loaders
{
	class TextureLoader
	{
	public:
		TextureLoader() = delete;

		static Texture* Create(std::string p_filePath, Rendering::Settings::ETextureFilteringMode p_minFilter, Rendering::Settings::ETextureFilteringMode p_magFilter, Rendering::Settings::EWrapMode p_wrapS, Rendering::Settings::EWrapMode p_wrapT, bool p_flipVertically, bool p_generateMipmap);
		static Texture* CreateColor(uint32_t p_data, Rendering::Settings::ETextureFilteringMode p_minFilter, Rendering::Settings::ETextureFilteringMode p_magFilter);
		static Texture* CreateDepth(uint32_t* p_data, uint16_t p_width, uint16_t p_height, Rendering::Settings::ETextureFilteringMode p_minFilter, Rendering::Settings::ETextureFilteringMode p_magFilter);
		static bool Destroy(Texture*& p_textureInstance);
		static bool Delete(Texture* p_textureInstance);

	private:
		static std::string FILE_TRACE;
	};
}
