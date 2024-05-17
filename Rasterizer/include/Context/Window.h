#pragma once

#include <string>

#include <SDL2/SDL_video.h>

#include "Context/Device.h"

#include "Context/Settings/WindowSettings.h"
#include "Context/Settings/ECursorMode.h"

namespace Context
{
	class Window
	{
	public:
		Eventing::Event<uint16_t, uint16_t> ResizeEvent;

		Window(Device& p_device, const Settings::WindowSettings& p_windowSettings);
		~Window();

		void UpdateWindowSurface() const;

		void Hide() const;
		void SetClose();
		void Restore() const;

		void SetCursorMode(Settings::ECursorMode p_cursorMode);
		Settings::ECursorMode GetCursorMode() const;

		void SetSize(uint16_t p_width, uint16_t p_height);

		bool IsActive()		const;
		bool isFullscreen() const;

		std::pair<uint16_t, uint16_t> GetSize() const;

		uint32_t GetWindowFlags() const;

		float GetAspectRatio() const;

		SDL_Window* GetSDLWindow() const;

	private:
		void CreateSDLWindow();

		void OnResizeWindow(uint16_t p_width, uint16_t p_height);

	private:
		Device& m_device;

		SDL_Window* m_sdlWindow;

		std::string m_title;
		std::pair<uint16_t, uint16_t> m_size;

		float m_aspectRatio;

		bool m_isFullscreen;
		bool m_isActive;

		Settings::ECursorMode m_cursorMode;

		uint32_t m_flags;
	};
}
