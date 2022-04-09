#include "Buffers/TextureBuffer.h"

Buffers::TextureBuffer::TextureBuffer(SDL_Renderer* p_sdlRenderer, uint32_t p_width, uint32_t p_height, uint32_t p_pixelFormat, uint32_t p_textureAccess) :
	m_sdlTexture(nullptr),
	m_bufferWidth(p_width),
	m_bufferHeight(p_height),
	m_pixelBufferRowSize(m_bufferWidth * sizeof(uint32_t)),
	m_bufferSize(m_bufferWidth * m_bufferHeight),
	m_pixelBuffer(new uint32_t[m_bufferSize])
{
	m_sdlTexture = SDL_CreateTexture(p_sdlRenderer, p_pixelFormat, p_textureAccess, static_cast<int>(m_bufferWidth), static_cast<int>(m_bufferHeight));
}

Buffers::TextureBuffer::~TextureBuffer()
{
	SDL_DestroyTexture(m_sdlTexture);
}

void Buffers::TextureBuffer::SendDataToGPU() const
{
	SDL_UpdateTexture(m_sdlTexture, nullptr, m_pixelBuffer, m_pixelBufferRowSize);
}

void Buffers::TextureBuffer::Clear(const Data::Color& p_color) const
{
	const uint32_t color = p_color.Pack();
	std::fill(m_pixelBuffer, m_pixelBuffer + m_bufferSize - 1, color);
}

void Buffers::TextureBuffer::SetPixel(uint32_t p_x, uint32_t p_y, const Data::Color& p_color) const
{
	m_pixelBuffer[p_y * m_bufferWidth + p_x] = p_color.Pack();
}

uint32_t Buffers::TextureBuffer::GetPixel(uint32_t p_x, uint32_t p_y) const
{
	return m_pixelBuffer[p_y * m_bufferWidth + p_x];
}

SDL_Texture* Buffers::TextureBuffer::GetSDLTexture() const
{
	return m_sdlTexture;
}
