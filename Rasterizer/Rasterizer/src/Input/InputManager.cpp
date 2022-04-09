#include "Input/InputManager.h"

#include <windows.h>

void InputManager::Update()
{
	m_keyPressedEvents.clear();
	m_keyRealesedEvents.clear();

	for (uint8_t i = 0; i < 255; ++i)
	{
		bool keyState = GetKeyState(i) & 0x8000;

		m_keyPressedEvents[static_cast<char>(i)]  = keyState && !m_keyStates[static_cast<char>(i)];
		m_keyRealesedEvents[static_cast<char>(i)] = !keyState && m_keyStates[static_cast<char>(i)];

		m_keyStates[static_cast<char>(i)] = keyState;
	}
}

bool InputManager::IsKeyPressed(char p_key)
{
	return m_keyStates[p_key];
}

bool InputManager::IsKeyPressOccured(char p_key)
{
	return m_keyPressedEvents[p_key];
}

bool InputManager::IsKeyRealeseOccured(char p_key)
{
	return m_keyRealesedEvents[p_key];
}