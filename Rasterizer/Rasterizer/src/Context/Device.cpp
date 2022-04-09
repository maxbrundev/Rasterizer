#include "Context/Device.h"

#include <stdexcept>

#include <SDL.h>

Context::Device::Device()
{
	InitSDL();
}

Context::Device::~Device()
{
	SDL_Quit();
}

void Context::Device::InitSDL() const
{
	const int8_t initializationCode = SDL_Init(SDL_INIT_VIDEO);
	
	if (initializationCode < 0)
	{
		SDL_Quit();
		throw std::runtime_error("Failed to Init SDL");
	}
}

void Context::Device::PushSDLEvent(SDL_Event p_event) const
{
	SDL_PushEvent(&p_event);
}

void Context::Device::HandleEvent()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if(event.type == SDL_WINDOWEVENT)
		{
			if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				ResizeEvent.Invoke(event.window.data1, event.window.data2);
			}
		}

		if (event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == SDLK_ESCAPE)
			{
				CloseEvent.Invoke();
			}
		}

		if (event.type == SDL_QUIT)
		{
			CloseEvent.Invoke();
		}
	}
}

uint32_t Context::Device::GetElapsedTime() const
{
	return SDL_GetTicks();
}
