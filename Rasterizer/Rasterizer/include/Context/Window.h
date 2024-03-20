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

		void Hide() const;
		void SetClose();
		void Restore() const;

		void SetCursorMode(ECursorMode p_cursorMode);
		ECursorMode GetCursorMode() const;

		void SetSize(uint16_t p_width, uint16_t p_height);

		bool IsActive()		const;
		bool isFullscreen() const;

		std::pair<uint16_t, uint16_t> GetSize() const;

		uint32_t GetWindowFlags() const;

		float GetAspectRatio() const;

		SDL_Surface* GetWindowSurface() const;
		SDL_Window* GetSDLWindow() const;

	private:
		void CreateSDLWindow();

		void OnResizeWindow(uint16_t p_width, uint16_t p_height);

	private:
		Device& m_device;

		SDL_Window* m_sdlWindow;
		SDL_Surface* m_surface;

		std::string m_title;
		std::pair<uint16_t, uint16_t> m_size;

		float m_aspectRatio;

		bool m_isFullscreen;
		bool m_isActive;

		ECursorMode m_cursorMode;

		uint32_t m_flags;
	};
}
