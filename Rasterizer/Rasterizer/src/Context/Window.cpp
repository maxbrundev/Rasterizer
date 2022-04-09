#include "Context/Window.h"

#include <SDL.h>

Context::Window::Window(Device& p_device, const Settings::WindowSettings& p_windowSettings) :
m_device(p_device),
m_sdlWindow(nullptr),
m_surface(nullptr),
m_title(p_windowSettings.title),
m_size(p_windowSettings.width, p_windowSettings.height),
m_fullscreen(p_windowSettings.fullScreen),
m_isActive(false),
m_cursorMode(ECursorMode::NORMAL)
{

	m_flags = m_fullscreen ? m_flags | SDL_WINDOW_FULLSCREEN : 0;
	m_flags = p_windowSettings.resizable ? m_flags | SDL_WINDOW_RESIZABLE : 0;

	CreateSDLWindow();
	UpdateSize();
	SetCursorMode(ECursorMode::NORMAL);

	p_device.CloseEvent.AddListener(std::bind(&Window::SetClose, this));
	p_device.ResizeEvent.AddListener(std::bind(&Window::UpdateSize, this));
}

Context::Window::~Window()
{
	SDL_FreeSurface(m_surface);
	SDL_DestroyWindow(m_sdlWindow);
}

void Context::Window::CreateSDLWindow()
{
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_size.first, m_size.second, m_flags);

	if (!m_sdlWindow)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to create GLFW Window");
	}

	m_surface = SDL_GetWindowSurface(m_sdlWindow);

	m_isActive = true;
}

void Context::Window::UpdateWindowSurface() const
{
	SDL_UpdateWindowSurface(m_sdlWindow);
}

void Context::Window::SetClose()
{
	m_isActive = false;
}

void Context::Window::SetCursorMode(ECursorMode p_cursorMode)
{
	m_cursorMode = p_cursorMode;
	SDL_ShowCursor(static_cast<int>(p_cursorMode));
}

SDL_Surface* Context::Window::GetWindowSurface()
{
	return m_surface;
}

Context::ECursorMode Context::Window::GetCursorMode() const
{
	return m_cursorMode;
}

uint32_t Context::Window::GetWindowFlags()
{
	return SDL_GetWindowFlags(m_sdlWindow);
}

void Context::Window::Restore() const
{
	SDL_RestoreWindow(m_sdlWindow);
}

void Context::Window::Hide() const
{
	SDL_HideWindow(m_sdlWindow);
}

bool Context::Window::IsActive() const
{
	return m_isActive;
}

bool Context::Window::isFullscreen() const
{
	return m_fullscreen;
}

SDL_Window* Context::Window::GetSDLWindow() const
{
	return m_sdlWindow;
}

void Context::Window::UpdateSize()
{
	int realWidth;
	int realHeight;
	
	SDL_GetWindowSize(m_sdlWindow, &realWidth, &realHeight);
	
	m_size.first  = realWidth;
	m_size.second = realHeight;
	
	m_halfSize.first  = m_size.first / 2;
	m_halfSize.second = m_size.second / 2;

	m_aspectRatio = static_cast<float>(m_size.first) / static_cast<float>(m_size.second);
}

std::pair<uint16_t, uint16_t> Context::Window::GetSize() const
{
	return m_size;
}