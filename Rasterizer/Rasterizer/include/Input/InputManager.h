#pragma once

#include <unordered_map>

class InputManager
{
public:
	InputManager() = default;

	void Update();

	bool IsKeyPressed(char p_key);
	bool IsKeyPressOccured(char p_key);
	bool IsKeyRealeseOccured(char p_key);

private:
	std::unordered_map<char, bool> m_keyPressedEvents;
	std::unordered_map<char, bool> m_keyRealesedEvents;
	std::unordered_map<char, bool> m_keyStates;
};