#include "Core/Renderer.h"

#include <SDL.h>

Core::Renderer::Renderer(Context::Window& p_window, const Settings::DriverSettings& p_driverSettings) : m_window(p_window),
m_sdlRenderer(nullptr)
{
	m_flags = p_driverSettings.vsync ? m_flags | SDL_RENDERER_PRESENTVSYNC : 0;
	m_flags |= SDL_RENDERER_ACCELERATED;

	InitRenderer();
}

Core::Renderer::~Renderer()
{
	SDL_DestroyRenderer(m_sdlRenderer);
}

void Core::Renderer::InitRenderer()
{
	m_sdlRenderer = SDL_CreateRenderer(m_window.GetSDLWindow(), -1, m_flags);
	if (!m_sdlRenderer)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL Renderer");
	}
}


void Core::Renderer::RenderClear() const
{
	SDL_RenderClear(m_sdlRenderer);
}

void Core::Renderer::RenderCopy(const Buffers::TextureBuffer& p_texture) const
{
	SDL_RenderCopy(m_sdlRenderer, p_texture.GetSDLTexture(), nullptr, nullptr);
}

void Core::Renderer::RenderPresent() const
{
	SDL_RenderPresent(m_sdlRenderer);
}
