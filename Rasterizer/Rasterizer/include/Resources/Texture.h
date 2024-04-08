#pragma once

#include <string>

namespace Resources::Loaders
{
	class TextureLoader;
}

namespace Resources
{
	class Texture
	{
		friend class Loaders::TextureLoader;

	public:
		Texture(std::string p_filePath, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, unsigned char* p_data);
		~Texture() = default;

	public:
		const std::string path;
		const uint32_t width;
		const uint32_t height;
		const uint32_t bitsPerPixel;
		uint8_t* data;
	};
}