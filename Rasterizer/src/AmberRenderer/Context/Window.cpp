#include "AmberRenderer/Context/Window.h"

#include <stdexcept>

#include <SDL2/SDL.h>

AmberRenderer::Context::Window::Window(Device& p_device, const Settings::WindowSettings& p_windowSettings) :
m_device(p_device),
m_sdlWindow(nullptr),
m_title(p_windowSettings.title),
m_size{p_windowSettings.width, p_windowSettings.height},
m_aspectRatio(static_cast<float>(p_windowSettings.width) / static_cast<float>(p_windowSettings.height)),
m_isFullscreen(p_windowSettings.fullScreen),
m_isActive(false),
m_cursorMode(Settings::ECursorMode::NORMAL),
m_flags(0)
{
	m_flags |= m_isFullscreen ? SDL_WINDOW_FULLSCREEN : 0;
	m_flags |= p_windowSettings.resizable ? SDL_WINDOW_RESIZABLE : 0;

	CreateSDLWindow();
	SetCursorMode(Settings::ECursorMode::NORMAL);

	p_device.CloseEvent.AddListener(std::bind(&Window::SetClose, this));
	p_device.ResizeEvent.AddListener(std::bind(&Window::OnResizeWindow, this, std::placeholders::_1, std::placeholders::_2));
}

AmberRenderer::Context::Window::~Window()
{
	SDL_DestroyWindow(m_sdlWindow);
}

void AmberRenderer::Context::Window::CreateSDLWindow()
{
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_size.first, m_size.second, m_flags);
	
	if (!m_sdlWindow)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to create GLFW Window");
	}

	m_isActive = true;
}

void AmberRenderer::Context::Window::OnResizeWindow(uint16_t p_width, uint16_t p_height)
{
	int width;
	int height;

	SDL_GetWindowSize(m_sdlWindow, &width, &height);

	m_size.first  = width;
	m_size.second = height;

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	ResizeEvent.Invoke(width, height);
}

void AmberRenderer::Context::Window::UpdateWindowSurface() const
{
	SDL_UpdateWindowSurface(m_sdlWindow);
}

void AmberRenderer::Context::Window::SetClose()
{
	m_isActive = false;
}

void AmberRenderer::Context::Window::SetCursorMode(Settings::ECursorMode p_cursorMode)
{
	m_cursorMode = p_cursorMode;
	SDL_ShowCursor(static_cast<int>(p_cursorMode));
}

AmberRenderer::Context::Settings::ECursorMode AmberRenderer::Context::Window::GetCursorMode() const
{
	return m_cursorMode;
}

void AmberRenderer::Context::Window::SetSize(uint16_t p_width, uint16_t p_height)
{
	m_size.first = p_width;
	m_size.second = p_height;

	SDL_SetWindowSize(m_sdlWindow, p_width, p_height);
}

uint32_t AmberRenderer::Context::Window::GetWindowFlags() const
{
	return SDL_GetWindowFlags(m_sdlWindow);
}

void AmberRenderer::Context::Window::Restore() const
{
	SDL_RestoreWindow(m_sdlWindow);
}

void AmberRenderer::Context::Window::Hide() const
{
	SDL_HideWindow(m_sdlWindow);
}

bool AmberRenderer::Context::Window::IsActive() const
{
	return m_isActive;
}

bool AmberRenderer::Context::Window::isFullscreen() const
{
	return m_isFullscreen;
}

SDL_Window* AmberRenderer::Context::Window::GetSDLWindow() const
{
	return m_sdlWindow;
}

float AmberRenderer::Context::Window::GetAspectRatio() const
{
	return m_aspectRatio;
}

std::pair<uint16_t, uint16_t> AmberRenderer::Context::Window::GetSize() const
{
	return m_size;
}