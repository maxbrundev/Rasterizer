#include "AmberRenderer/Context/SDLDriver.h"

#include <stdexcept>

#include <SDL2/SDL.h>

AmberRenderer::Context::SDLDriver::SDLDriver(Window& p_window, const Settings::SDLDriverSettings& p_driverSettings) : m_window(p_window)
{
	m_flags = p_driverSettings.vsync ? SDL_RENDERER_PRESENTVSYNC : 0;
	m_flags |= SDL_RENDERER_ACCELERATED;

	CreateRenderer();

	m_isActive = true;
}

AmberRenderer::Context::SDLDriver::~SDLDriver()
{
	SDL_DestroyRenderer(m_sdlRenderer);

}

void AmberRenderer::Context::SDLDriver::RenderClear() const
{
	SDL_RenderClear(m_sdlRenderer);
}

void AmberRenderer::Context::SDLDriver::RenderCopy(SDL_Texture* p_texture) const
{
	SDL_RenderCopy(m_sdlRenderer, p_texture, nullptr, nullptr);
}

void AmberRenderer::Context::SDLDriver::RenderPresent() const
{
	SDL_RenderPresent(m_sdlRenderer);
}

void AmberRenderer::Context::SDLDriver::CreateRenderer()
{
	m_sdlRenderer = SDL_CreateRenderer(m_window.GetSDLWindow(), -1, m_flags);

	if (!m_sdlRenderer)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL Renderer");
	}
}

bool AmberRenderer::Context::SDLDriver::IsActive() const
{
	return m_isActive;
}

SDL_Renderer* AmberRenderer::Context::SDLDriver::GetRenderer() const
{
	return m_sdlRenderer;
}
