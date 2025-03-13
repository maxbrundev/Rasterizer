#pragma once

#include <string>
#include <vector>

#include "AmberRenderer/Resources/Settings/ETextureFilteringMode.h"
#include "AmberRenderer/Resources/Settings/ETextureWrapMode.h"

namespace AmberRenderer::Resources::Loaders
{
	class TextureLoader;
}

namespace AmberRenderer::Rendering
{
	class Renderer;
}

namespace AmberRenderer::Resources
{
	class Texture
	{
		friend class Loaders::TextureLoader;

	public:
		Texture(const std::string& p_filePath, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter, Settings::ETextureWrapMode p_wrapS, Settings::ETextureWrapMode p_wrapT, bool p_generateMipmap);
		~Texture() = default;
		
		void Bind(uint32_t p_slot = 0) const;
		void Unbind() const;

	public:
		const uint32_t ID;
		const std::string Path;
		const uint32_t Width;
		const uint32_t Height;
		const uint32_t BitsPerPixel;
		const Settings::ETextureFilteringMode MinFilter;
		const Settings::ETextureFilteringMode MagFilter;
		const Settings::ETextureWrapMode WrapS;
		const Settings::ETextureWrapMode WrapT;

		uint8_t* Data;

		bool HasMipmaps;
	};
}