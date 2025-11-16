#pragma once

#include <SDL2/SDL_render.h>

#include "OnyxEditor/Context/Window.h"

#include "OnyxEditor/Context/Settings/SDLDriverSettings.h"

namespace OnyxEditor::Context
{
	class SDLDriver
	{
	public:
		SDLDriver(Window& p_window, const Settings::SDLDriverSettings& p_driverSettings);
		~SDLDriver();

		void RenderClear() const;
		void RenderCopy(SDL_Texture* p_texture) const;
		void RenderPresent() const;

		SDL_Texture* CreateTexture(uint32_t format, int access, int width, int height) const;
		void DestroyTexture(SDL_Texture* p_sdlTexture);

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
