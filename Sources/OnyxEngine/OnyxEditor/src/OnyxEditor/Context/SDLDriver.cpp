#include "OnyxEditor/Context/SDLDriver.h"

#include <stdexcept>

#include <SDL2/SDL.h>

OnyxEditor::Context::SDLDriver::SDLDriver(Window& p_window, const Settings::SDLDriverSettings& p_driverSettings) : m_window(p_window)
{
	m_flags = p_driverSettings.vsync ? SDL_RENDERER_PRESENTVSYNC : 0;
	m_flags |= SDL_RENDERER_ACCELERATED;

	CreateRenderer();

	m_isActive = true;
}

OnyxEditor::Context::SDLDriver::~SDLDriver()
{
	SDL_DestroyRenderer(m_sdlRenderer);
}

void OnyxEditor::Context::SDLDriver::RenderClear() const
{
	SDL_RenderClear(m_sdlRenderer);
}

void OnyxEditor::Context::SDLDriver::RenderCopy(SDL_Texture* p_texture) const
{
	SDL_RenderCopy(m_sdlRenderer, p_texture, nullptr, nullptr);
}

void OnyxEditor::Context::SDLDriver::RenderPresent() const
{
	SDL_RenderPresent(m_sdlRenderer);
}

SDL_Texture* OnyxEditor::Context::SDLDriver::CreateTexture(uint32_t format, int access, int width, int height) const
{
	return SDL_CreateTexture(m_sdlRenderer, format, access, width, height);
}

void OnyxEditor::Context::SDLDriver::DestroyTexture(SDL_Texture* p_sdlTexture)
{
	SDL_DestroyTexture(p_sdlTexture);
}

void OnyxEditor::Context::SDLDriver::CreateRenderer()
{
	m_sdlRenderer = SDL_CreateRenderer(m_window.GetSDLWindow(), -1, m_flags);

	if (!m_sdlRenderer)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL Renderer");
	}
}

bool OnyxEditor::Context::SDLDriver::IsActive() const
{
	return m_isActive;
}

SDL_Renderer* OnyxEditor::Context::SDLDriver::GetRenderer() const
{
	return m_sdlRenderer;
}
