#pragma once

#include <cstdint>

#include <SDL_render.h>

#include "Data/Color.h"

namespace Buffers
{
	class TextureBuffer
	{
	public:
		TextureBuffer(SDL_Renderer* p_sdlRenderer, uint32_t p_width, uint32_t p_height, uint32_t p_pixelFormat, uint32_t p_textureAccess);
		~TextureBuffer();

		void SendDataToGPU() const;

		void Clear(const Data::Color& p_color) const;

		void SetPixel(int p_x, int p_y, const Data::Color& p_color) const;
		uint32_t GetPixel(uint32_t p_x, uint32_t p_y) const;

		SDL_Texture* GetSDLTexture() const;

	private:
		SDL_Texture* m_sdlTexture;
		
		const uint32_t m_width;
		const uint32_t m_height;
		const uint32_t m_size;
		const uint32_t m_rowSize;
		const uint32_t m_sizeInByte;

		uint32_t* m_data;
	};
}
