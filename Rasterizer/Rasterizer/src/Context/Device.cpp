#include "Context/Device.h"

#include <SDL2/SDL.h>

Context::Device::Device()
{
	InitializeSDL();
}

void Context::Device::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		SDL_Quit();

		throw std::runtime_error("Failed to Init SDL");
	}
}
	
Context::Device::~Device()
{
	SDL_Quit();
}

void Context::Device::PollEvents()
{
	MouseMovedEvent.Invoke(std::make_pair(0, 0));
	MouseWheelEvent.Invoke(0);

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type) 
		{
		case SDL_KEYDOWN:
			KeyPressedEvent.Invoke(event.key.keysym.scancode);
			break;

		case SDL_KEYUP:
			KeyReleasedEvent.Invoke(event.key.keysym.sym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			MouseButtonPressedEvent.Invoke(event.button.button);
			break;

		case SDL_MOUSEBUTTONUP:
			MouseButtonReleasedEvent.Invoke(event.button.button);
			break;

		case SDL_QUIT:
			CloseEvent.Invoke();
			break;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) 
			{
				ResizeEvent.Invoke(event.window.data1, event.window.data2);
			}
			break;

		case SDL_MOUSEMOTION:
			MouseMovedEvent.Invoke(std::make_pair(event.motion.xrel, event.motion.yrel));
			break;

		case SDL_MOUSEWHEEL:
			MouseWheelEvent.Invoke(event.wheel.y);
			break;

		default: 
			break;
		}
	}
}

void Context::Device::SetRelativeMouseMode(bool p_value)
{
	SDL_SetRelativeMouseMode(p_value ? SDL_TRUE : SDL_FALSE);
}

bool Context::Device::GetRelativeMouseMode()
{
	return SDL_GetRelativeMouseMode();
}

uint32_t Context::Device::GetTicks() const
{
	return SDL_GetTicks();
}

