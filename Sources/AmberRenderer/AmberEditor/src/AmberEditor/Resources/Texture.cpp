#include "AmberEditor/Resources/Texture.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

AmberEditor::Resources::Texture::Texture(const std::string& p_filePath, uint32_t p_id, uint32_t p_width, uint32_t p_height, uint32_t p_bitsPerPixel, Rendering::Settings::ETextureFilteringMode p_minFilter, Rendering::Settings::ETextureFilteringMode p_magFilter, Rendering::Settings::EWrapMode p_wrapS, Rendering::Settings::EWrapMode p_wrapT, bool p_generateMipmap) :
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

void AmberEditor::Resources::Texture::Bind(uint32_t p_slot) const
{
	AmberGL::ActiveTexture(AGL_TEXTURE0 + p_slot);
	AmberGL::BindTexture(AGL_TEXTURE_2D, ID);
}

void AmberEditor::Resources::Texture::Unbind() const
{
	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);
}
