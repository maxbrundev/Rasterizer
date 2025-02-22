#include "Context/SDLDriver.h"

#include <stdexcept>

#include <SDL2/SDL.h>

Context::SDLDriver::SDLDriver(Window& p_window, const Settings::DriverSettings& p_driverSettings) : m_window(p_window)
{
	m_flags = p_driverSettings.vsync ? SDL_RENDERER_PRESENTVSYNC : 0;
	m_flags |= SDL_RENDERER_ACCELERATED;

	CreateRenderer();

	m_isActive = true;
}

Context::SDLDriver::~SDLDriver()
{
	SDL_DestroyRenderer(m_sdlRenderer);

}

void Context::SDLDriver::RenderClear() const
{
	SDL_RenderClear(m_sdlRenderer);
}

void Context::SDLDriver::RenderCopy(SDL_Texture* p_texture) const
{
	SDL_RenderCopy(m_sdlRenderer, p_texture, nullptr, nullptr);
}

void Context::SDLDriver::RenderPresent() const
{
	SDL_RenderPresent(m_sdlRenderer);
}

void Context::SDLDriver::CreateRenderer()
{
	m_sdlRenderer = SDL_CreateRenderer(m_window.GetSDLWindow(), -1, m_flags);

	if (!m_sdlRenderer)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL Renderer");
	}
}

bool Context::SDLDriver::IsActive() const
{
	return m_isActive;
}

SDL_Renderer* Context::SDLDriver::GetRenderer() const
{
	return m_sdlRenderer;
}
