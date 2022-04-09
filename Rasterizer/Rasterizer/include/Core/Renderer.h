#pragma once

#include <SDL_render.h>

#include "Settings/DriverSettings.h"

#include "Context/Window.h"

#include "Buffers/TextureBuffer.h"

namespace Core
{
	class Renderer
	{
	public:
		Renderer(Context::Window& p_window, const Settings::DriverSettings& p_driverSettings);
		~Renderer();

		void RenderClear() const;
		void RenderCopy(const Buffers::TextureBuffer& p_texture) const;
		void RenderPresent() const;

	private:
		void InitRenderer();

	public:
		Context::Window& m_window;

		SDL_Renderer* m_sdlRenderer;

		uint32_t m_flags = 0;
	};
}
