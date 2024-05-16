#include "Context/Driver.h"

#include <stdexcept>

#include <SDL2/SDL.h>

Context::Driver::Driver(Window& p_window, const Settings::DriverSettings& p_driverSettings) : m_window(p_window)
{
	m_flags = p_driverSettings.vsync ? m_flags | SDL_RENDERER_PRESENTVSYNC : 0;
	m_flags |= SDL_RENDERER_ACCELERATED;

	CreateRenderer();

	m_isActive = true;
}

Context::Driver::~Driver()
{
	SDL_DestroyRenderer(m_sdlRenderer);

}

void Context::Driver::RenderClear() const
{
	SDL_RenderClear(m_sdlRenderer);
}

void Context::Driver::RenderCopy(SDL_Texture* p_texture) const
{
	SDL_RenderCopy(m_sdlRenderer, p_texture, nullptr, nullptr);
}

void Context::Driver::RenderPresent() const
{
	SDL_RenderPresent(m_sdlRenderer);
}

void Context::Driver::CreateRenderer()
{
	m_sdlRenderer = SDL_CreateRenderer(m_window.GetSDLWindow(), -1, m_flags);

	if (!m_sdlRenderer)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL Renderer");
	}
}

bool Context::Driver::IsActive() const
{
	return m_isActive;
}

SDL_Renderer* Context::Driver::GetRenderer() const
{
	return m_sdlRenderer;
}
