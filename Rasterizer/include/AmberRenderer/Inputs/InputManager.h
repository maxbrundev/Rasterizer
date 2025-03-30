#pragma once

#include <unordered_map>

#include "AmberRenderer/Context/Window.h"

#include "AmberRenderer/Inputs/EKey.h"
#include "AmberRenderer/Inputs/EKeyState.h"
#include "AmberRenderer/Inputs/EMouseButton.h"
#include "AmberRenderer/Inputs/EMouseButtonState.h"

namespace AmberRenderer::Inputs
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
		std::pair<int, int> GetMouseRelativeMovement() const;
		int GetMouseWheel() const;

		void LockMouse() const;

		void UnlockMouse() const;

		bool IsMouseLocked() const;

		void ClearEvents();

	private:
		void OnKeyPressed(int p_key);
		void OnKeyReleased(int p_key);
		void OnMouseButtonPressed(int p_button);
		void OnMouseButtonReleased(int p_button);
		void OnMouseMoved(std::pair<int, int> p_motion);
		void OnMouseWheel(int p_wheel);

	private:
		Context::Device& m_device;

		uint64_t m_keyPressedListener;
		uint64_t m_keyReleasedListener;
		uint64_t m_mouseButtonPressedListener;
		uint64_t m_mouseButtonReleasedListener;
		uint64_t m_mouseMovedListener;

		std::unordered_map<EKey, EKeyState>					m_keyEvents;
		std::unordered_map<EMouseButton, EMouseButtonState>	m_mouseButtonEvents;

		std::pair<int, int> m_mouseRelativeMovement;
		int m_mouseWheel;
	};
}