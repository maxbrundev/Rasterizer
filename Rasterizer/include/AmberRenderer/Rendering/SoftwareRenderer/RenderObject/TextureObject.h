#pragma once

#include <cstdint>

namespace AmberRenderer::Rendering::SoftwareRenderer::RenderObject
{
	struct TextureObject
	{
		uint32_t ID;
		uint32_t Target;
		uint32_t InternalFormat;
		uint32_t Width;
		uint32_t Height;

		union
		{
			uint8_t* Data8;
			//TODO: Investigate the necessary of a 32 bit buffer.
			//float* Data32;
		};

		uint8_t MinFilter;
		uint8_t MagFilter;
		uint8_t WrapS;
		uint8_t WrapT;

		uint8_t** Mipmaps;
	};
}
