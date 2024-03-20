#pragma once

#include "Context/Window.h"

#include "Settings/DriverSettings.h"

#include "Buffers/TextureBuffer.h"


namespace Context
{
	class Driver
	{
	public:
		Driver(Window& p_window, const Settings::DriverSettings& p_driverSettings);
		~Driver();

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
