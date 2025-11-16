#include "OnyxEditor/Context/SDLDisplay.h"

#include "AmberGL/SoftwareRenderer/AmberGL.h"

OnyxEditor::Context::SDLDisplay::SDLDisplay(Context::SDLDriver& p_driver, uint16_t p_width, uint16_t p_height) :
m_driver(p_driver),
m_displayTexture(nullptr),
m_size{p_width, p_height}
{
	m_displayTexture = m_driver.CreateTexture(SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, p_width, p_height);
}

OnyxEditor::Context::SDLDisplay::~SDLDisplay()
{
	m_driver.DestroyTexture(m_displayTexture);
}

void OnyxEditor::Context::SDLDisplay::Clear() const
{
	m_driver.RenderClear();
}

void OnyxEditor::Context::SDLDisplay::Present() const
{
	m_driver.RenderCopy(m_displayTexture);
	m_driver.RenderPresent();
}

void OnyxEditor::Context::SDLDisplay::UpdateDisplayTexture(const void* p_data, uint32_t p_rowSize) const
{
	SDL_UpdateTexture(m_displayTexture, nullptr, p_data, p_rowSize);
}

void OnyxEditor::Context::SDLDisplay::Resize(uint16_t p_width, uint16_t p_height)
{
	if (m_size.first == p_width && m_size.second == p_height) 
		return;

	AmberGL::ResizeDefaultFramebuffer(p_width, p_height);

	if (m_displayTexture) m_driver.DestroyTexture(m_displayTexture);
	
	m_displayTexture = m_driver.CreateTexture(SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, p_width, p_height);
	
	m_size = {p_width, p_height};
}

std::pair<uint16_t, uint16_t> OnyxEditor::Context::SDLDisplay::GetSize() const
{
	return m_size;
}
