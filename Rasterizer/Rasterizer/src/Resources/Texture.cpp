#include "Resources/Texture.h"

#include <algorithm>

Resources::Texture::Texture(std::string p_filePath, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, unsigned char* p_data, ETextureFilteringMode p_filter, ETextureWrapMode p_wrapping) :
path(std::move(p_filePath)),
width(p_width),
height(p_width),
bitsPerPixel(p_bitsPerPixel),
Filter(p_filter),
Wrapping(p_wrapping)
{
	data = new uint8_t[width * height * 4];

	std::memcpy(data, p_data, width * height * 4);
}

Resources::Texture::~Texture()
{
	delete[] data;
	data = nullptr;
}
