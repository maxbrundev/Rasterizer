#include "Buffers/TextureBuffer.h"

Buffers::TextureBuffer::TextureBuffer(SDL_Renderer* p_sdlRenderer, uint32_t p_width, uint32_t p_height, uint32_t p_pixelFormat, uint32_t p_textureAccess) :
	m_sdlTexture(nullptr),
	m_width(p_width),
	m_height(p_height),
	m_size(m_width * m_height),
	m_rowSize(m_width * sizeof(uint32_t)),
	m_sizeInByte(m_size * sizeof(uint32_t)),
	m_data(new uint32_t[m_size])
{
	m_sdlTexture = SDL_CreateTexture(p_sdlRenderer, p_pixelFormat, p_textureAccess, static_cast<int>(m_width), static_cast<int>(m_height));
}

Buffers::TextureBuffer::~TextureBuffer()
{
	delete[] m_data;
	m_data = nullptr;

	SDL_DestroyTexture(m_sdlTexture);
}

void Buffers::TextureBuffer::SendDataToGPU() const
{
	SDL_UpdateTexture(m_sdlTexture, nullptr, m_data, m_rowSize);
}

void Buffers::TextureBuffer::Clear(const Data::Color& p_color) const
{
	const uint32_t color = p_color.Pack();
	std::fill(m_data, m_data + m_size - 1, color);
}

void Buffers::TextureBuffer::SetPixel(int p_x, int p_y, const Data::Color& p_color) const
{
	m_data[p_y * m_width + p_x] = p_color.Pack();
}

uint32_t Buffers::TextureBuffer::GetPixel(uint32_t p_x, uint32_t p_y) const
{
	return m_data[p_y * m_width + p_x];
}

SDL_Texture* Buffers::TextureBuffer::GetSDLTexture() const
{
	return m_sdlTexture;
}
