#pragma once

#include <SDL.h>

#include "Eventing/Event.h"

namespace Context
{
	class Device
	{
	public:
		Eventing::Event<int> KeyPressedEvent;
		Eventing::Event<int> KeyReleasedEvent;
		Eventing::Event<int> MouseButtonPressedEvent;
		Eventing::Event<int> MouseButtonReleasedEvent;

		Eventing::Event<> CloseEvent;

		Eventing::Event<uint16_t, uint16_t> ResizeEvent;

		Device();
		~Device();

		void PollEvents();

		uint32_t GetTicks() const;

	private:
		void InitializeSDL();
	};
}
