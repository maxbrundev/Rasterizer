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

		void SetPixel(uint32_t p_x, uint32_t p_y, const Data::Color& p_color) const;
		uint32_t GetPixel(uint32_t p_x, uint32_t p_y) const;

		SDL_Texture* GetSDLTexture() const;

	private:
		SDL_Texture* m_sdlTexture;
		
		const uint32_t m_bufferWidth;
		const uint32_t m_bufferHeight;
		const uint32_t m_pixelBufferRowSize;
		const uint32_t m_bufferSize;
		uint32_t* m_pixelBuffer;
	};
}
