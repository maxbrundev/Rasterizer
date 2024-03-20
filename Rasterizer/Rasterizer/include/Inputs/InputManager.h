#pragma once

#include <unordered_map>

#include "Context/Window.h"

#include "Inputs/EKey.h"
#include "Inputs/EKeyState.h"
#include "Inputs/EMouseButton.h"
#include "Inputs/EMouseButtonState.h"

namespace Inputs
{
	class InputManager
	{
	public:
		InputManager(Context::Device& p_window);
		~InputManager();

		EKeyState GetKeyState(EKey p_key) const;

		EMouseButtonState GetMouseButtonState(EMouseButton p_button) const;

		bool IsKeyPressed(EKey p_key) const;

		bool IsKeyReleased(EKey p_key) const;

		bool IsMouseButtonPressed(EMouseButton p_button) const;

		bool IsMouseButtonReleased(EMouseButton p_button) const;

		std::pair<int, int> GetMousePosition() const;

		void ClearEvents();

	private:
		void OnKeyPressed(int p_key);
		void OnKeyReleased(int p_key);
		void OnMouseButtonPressed(int p_button);
		void OnMouseButtonReleased(int p_button);

	private:
		Context::Device& m_device;

		uint64_t m_keyPressedListener;
		uint64_t m_keyReleasedListener;
		uint64_t m_mouseButtonPressedListener;
		uint64_t m_mouseButtonReleasedListener;

		std::unordered_map<EKey, EKeyState>					m_keyEvents;
		std::unordered_map<EMouseButton, EMouseButtonState>	m_mouseButtonEvents;
	};
}