#pragma once

#include <SDL_events.h>

#include "Eventing/Event.h"

namespace Context
{
	class Device
	{
	public:
		Device();
		~Device();

		void PushSDLEvent(SDL_Event p_event) const;
		void HandleEvent();

		uint32_t GetElapsedTime() const;

	public:
		Eventing::Event<> CloseEvent;
		Eventing::Event<uint16_t, uint16_t> ResizeEvent;

	private:
		void InitSDL() const;
	};
}
