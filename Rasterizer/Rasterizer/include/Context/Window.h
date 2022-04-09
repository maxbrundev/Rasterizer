#pragma once

#include "Settings/WindowSettings.h"

#include "Context/Device.h"
#include "Context/ECursorMode.h"

namespace Context
{
	class Window
	{
	public:
		Window(Device& p_device, const Settings::WindowSettings& p_windowSettings);
		~Window();

		void UpdateWindowSurface() const;
		void Restore() const;
		void Hide() const;
		void SetClose();

		void SetCursorMode(Context::ECursorMode p_cursorMode);
		void UpdateSize();

		bool IsActive()		const;
		bool isFullscreen() const;

		SDL_Surface* GetWindowSurface();
		SDL_Window* GetSDLWindow() const;

		std::pair<uint16_t, uint16_t> GetSize() const;

		ECursorMode GetCursorMode() const;

		uint32_t GetWindowFlags();

	private:
		void CreateSDLWindow();

	private:
		Device& m_device;

		SDL_Window* m_sdlWindow;
		SDL_Surface* m_surface;

		std::string m_title;

		uint32_t m_flags = 0;
		std::pair<uint16_t, uint16_t> m_size;
		std::pair<uint16_t, uint16_t> m_halfSize;

		float m_aspectRatio;

		bool m_fullscreen;
		bool m_isActive;

		ECursorMode m_cursorMode;
	};
}
