#pragma once

#include <string>

#include "AmberEditor/Eventing/Event.h"

namespace AmberEditor::Context
{
	class Device
	{
	public:
		Eventing::Event<int> KeyPressedEvent;
		Eventing::Event<int> KeyReleasedEvent;
		Eventing::Event<int> MouseButtonPressedEvent;
		Eventing::Event<int> MouseButtonReleasedEvent;
		Eventing::Event<std::pair<int, int>> MouseMovedEvent;
		Eventing::Event<int> MouseWheelEvent;
		Eventing::Event<std::string> DropFileEvent;

		Eventing::Event<> CloseEvent;

		Eventing::Event<uint16_t, uint16_t> ResizeEvent;

		Device();
		~Device();

		void PollEvents();

		void SetRelativeMouseMode(bool p_value);
		bool GetRelativeMouseMode();

		uint32_t GetTicks() const;

	private:
		void InitializeSDL();
	};
}
