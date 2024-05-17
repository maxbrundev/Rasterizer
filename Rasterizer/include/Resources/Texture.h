#pragma once

#include <string>
#include <vector>

#include "Resources/Settings/ETextureFilteringMode.h"
#include "Resources/Settings/ETextureWrapMode.h"

namespace Resources::Loaders
{
	class TextureLoader;
}

namespace Resources
{
	class Texture
	{
		friend class Loaders::TextureLoader;

		struct MipmapsData
		{
			uint8_t* Data;
			uint32_t Width;
			uint32_t Height;

			MipmapsData(uint8_t* p_data, uint32_t p_width, uint32_t p_height) : Data(p_data), Width(p_width), Height(p_height)
			{
			}
		};

	public:
		Texture(std::string p_filePath, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, unsigned char* p_data, Settings::ETextureFilteringMode p_filter, Settings::ETextureWrapMode p_wrapping, bool p_generateMipmap);
		~Texture();

	private:
		void GenerateMipmaps();

	public:
		const std::string Path;
		const uint32_t Width;
		const uint32_t Height;
		const uint32_t BitsPerPixel;
		uint8_t* Data;
		const Settings::ETextureFilteringMode Filter;
		const Settings::ETextureWrapMode Wrapping;
		bool HasMipmaps;
		std::vector<MipmapsData> Mipmaps;
	};
}