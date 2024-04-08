#include "Resources/Texture.h"

#include <algorithm>
#include <glm/vec4.hpp>

Resources::Texture::Texture(std::string p_filePath, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, unsigned char* p_data) :
path(std::move(p_filePath)),
width(p_width),
height(p_width),
bitsPerPixel(p_bitsPerPixel)
{
	data = new uint8_t[width * height * 4];

	std::memcpy(data, p_data, width * height * 4);
}
