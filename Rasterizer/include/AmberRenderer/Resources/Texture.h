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