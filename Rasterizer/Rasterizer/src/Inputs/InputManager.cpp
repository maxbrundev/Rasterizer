#include "Inputs/InputManager.h"

Inputs::InputManager::InputManager(Context::Device& p_device) : m_device(p_device)
{
	m_keyPressedListener = m_device.KeyPressedEvent.AddListener(std::bind(&InputManager::OnKeyPressed, this, std::placeholders::_1));
	m_keyReleasedListener = m_device.KeyReleasedEvent.AddListener(std::bind(&InputManager::OnKeyReleased, this, std::placeholders::_1));
	m_mouseButtonPressedListener = m_device.MouseButtonPressedEvent.AddListener(std::bind(&InputManager::OnMouseButtonPressed, this, std::placeholders::_1));
	m_mouseButtonReleasedListener = m_device.MouseButtonReleasedEvent.AddListener(std::bind(&InputManager::OnMouseButtonReleased, this, std::placeholders::_1));
}

Inputs::InputManager::~InputManager()
{
	m_device.KeyPressedEvent.RemoveListener(m_keyPressedListener);
	m_device.KeyReleasedEvent.RemoveListener(m_keyReleasedListener);
	m_device.MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListener);
	m_device.MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListener);
}

Inputs::EKeyState Inputs::InputManager::GetKeyState(EKey p_key) const
{
	SDL_PumpEvents();

	const uint8_t* state = SDL_GetKeyboardState(NULL);

	switch (state[static_cast<uint8_t>(p_key)])
	{
		case SDL_PRESSED:	return EKeyState::KEY_DOWN;
		case SDL_RELEASED:	return EKeyState::KEY_UP;
	}

	return EKeyState::KEY_UP;
}

Inputs::EMouseButtonState Inputs::InputManager::GetMouseButtonState(EMouseButton p_button) const
{
	uint32_t state = SDL_GetMouseState(nullptr, nullptr);

	if (state & SDL_BUTTON(static_cast<int>(p_button)))
	{
		return EMouseButtonState::MOUSE_DOWN;
	}

	return EMouseButtonState::MOUSE_UP;
}

bool Inputs::InputManager::IsKeyPressed(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_DOWN;
}

bool Inputs::InputManager::IsKeyReleased(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_UP;
}

bool Inputs::InputManager::IsMouseButtonPressed(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_DOWN;
}

bool Inputs::InputManager::IsMouseButtonReleased(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_UP;
}

std::pair<int, int> Inputs::InputManager::GetMousePosition() const
{
	std::pair<int, int> result;
	SDL_GetMouseState(&result.first, &result.second);
	return result;
}

void Inputs::InputManager::ClearEvents()
{
	m_keyEvents.clear();
	m_mouseButtonEvents.clear();
}

void Inputs::InputManager::OnKeyPressed(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_DOWN;
}

void Inputs::InputManager::OnKeyReleased(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_UP;
}

void Inputs::InputManager::OnMouseButtonPressed(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_DOWN;
}

void Inputs::InputManager::OnMouseButtonReleased(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_UP;
}
