#include "AmberEditor/Context/SDLDisplay.h"

AmberEditor::Context::SDLDisplay::SDLDisplay(Context::SDLDriver& p_driver, uint16_t p_width, uint16_t p_height) :
m_driver(p_driver),
m_displayTexture(nullptr),
m_size{p_width, p_height}
{
	m_displayTexture = SDL_CreateTexture(m_driver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, p_width, p_height);
}

AmberEditor::Context::SDLDisplay::~SDLDisplay()
{
	SDL_DestroyTexture(m_displayTexture);
}

void AmberEditor::Context::SDLDisplay::Clear() const
{
	m_driver.RenderClear();
}

void AmberEditor::Context::SDLDisplay::Present() const
{
	m_driver.RenderCopy(m_displayTexture);
	m_driver.RenderPresent();
}

void AmberEditor::Context::SDLDisplay::UpdateDisplayTexture(const void* p_data, uint32_t p_rowSize) const
{
	SDL_UpdateTexture(m_displayTexture, nullptr, p_data, p_rowSize);
}

std::pair<uint16_t, uint16_t> AmberEditor::Context::SDLDisplay::GetSize() const
{
	return m_size;
}
