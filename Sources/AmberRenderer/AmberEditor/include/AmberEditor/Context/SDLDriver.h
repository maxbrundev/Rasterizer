#pragma once

#include <SDL2/SDL_render.h>

#include "AmberEditor/Context/Window.h"

#include "AmberEditor/Context/Settings/SDLDriverSettings.h"

namespace AmberEditor::Context
{
	class SDLDriver
	{
	public:
		SDLDriver(Window& p_window, const Settings::SDLDriverSettings& p_driverSettings);
		~SDLDriver();

		void RenderClear() const;
		void RenderCopy(SDL_Texture* p_texture) const;
		void RenderPresent() const;

		bool IsActive() const;

		SDL_Renderer* GetRenderer() const;

	private:
		void CreateRenderer();

	private:
		Window& m_window;

		SDL_Renderer* m_sdlRenderer;

		bool m_isActive;

		uint32_t m_flags;
	};
}
