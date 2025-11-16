#pragma once

#include <string>

#include "OnyxEditor/Rendering/Settings/ETextureFilteringMode.h"
#include "OnyxEditor/Rendering/Settings/EWrapMode.h"

namespace OnyxEditor::Resources::Loaders
{
	class TextureLoader;
}

namespace OnyxEditor::Resources
{
	class Texture
	{
		friend class Loaders::TextureLoader;

	public:
		Texture(const std::string& p_filePath, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, Rendering::Settings::ETextureFilteringMode p_minFilter, Rendering::Settings::ETextureFilteringMode p_magFilter, Rendering::Settings::EWrapMode p_wrapS, Rendering::Settings::EWrapMode p_wrapT, bool p_generateMipmap);
		~Texture() = default;
		
		void Bind(uint32_t p_slot = 0) const;
		void Unbind(uint32_t p_slot) const;

	public:
		const uint32_t ID;
		const std::string Path;
		const uint32_t Width;
		const uint32_t Height;
		const uint32_t BitsPerPixel;
		const Rendering::Settings::ETextureFilteringMode MinFilter;
		const Rendering::Settings::ETextureFilteringMode MagFilter;
		const Rendering::Settings::EWrapMode WrapS;
		const Rendering::Settings::EWrapMode WrapT;
		const bool HasMipmaps;
	};
}