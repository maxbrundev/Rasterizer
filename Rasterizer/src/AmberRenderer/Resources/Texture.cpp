#include "AmberRenderer/Resources/Texture.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

AmberRenderer::Resources::Texture::Texture(const std::string& p_filePath, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, Settings::ETextureFilteringMode p_minFilter, Settings::ETextureFilteringMode p_magFilter, Settings::ETextureWrapMode p_wrapS, Settings::ETextureWrapMode p_wrapT, bool p_generateMipmap) :
ID(p_id),
Path(p_filePath),
Width(p_width),
Height(p_height),
BitsPerPixel(p_bitsPerPixel),
MinFilter(p_minFilter),
MagFilter(p_magFilter),
WrapS(p_wrapS),
WrapT(p_wrapT),
HasMipmaps(p_generateMipmap)
{
}

void AmberRenderer::Resources::Texture::Bind(uint32_t p_slot) const
{
	GLRasterizer::ActiveTexture(GLR_TEXTURE0 + p_slot);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, ID);
}

void AmberRenderer::Resources::Texture::Unbind() const
{
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);
}
