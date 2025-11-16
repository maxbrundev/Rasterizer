#include "OnyxEditor/Inputs/InputManager.h"

#include <SDL2/SDL_events.h>

OnyxEditor::Inputs::InputManager::InputManager(Context::Device& p_device) : m_device(p_device)
{
	m_keyPressedListener = m_device.KeyPressedEvent.AddListener(std::bind(&InputManager::OnKeyPressed, this, std::placeholders::_1));
	m_keyReleasedListener = m_device.KeyReleasedEvent.AddListener(std::bind(&InputManager::OnKeyReleased, this, std::placeholders::_1));
	m_mouseButtonPressedListener = m_device.MouseButtonPressedEvent.AddListener(std::bind(&InputManager::OnMouseButtonPressed, this, std::placeholders::_1));
	m_mouseButtonReleasedListener = m_device.MouseButtonReleasedEvent.AddListener(std::bind(&InputManager::OnMouseButtonReleased, this, std::placeholders::_1));
	m_mouseMovedListener = m_device.MouseMovedEvent.AddListener(std::bind(&InputManager::OnMouseMoved, this, std::placeholders::_1));
	m_device.MouseWheelEvent.AddListener(std::bind(&InputManager::OnMouseWheel, this, std::placeholders::_1));
}

OnyxEditor::Inputs::InputManager::~InputManager()
{
	m_device.KeyPressedEvent.RemoveListener(m_keyPressedListener);
	m_device.KeyReleasedEvent.RemoveListener(m_keyReleasedListener);
	m_device.MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListener);
	m_device.MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListener);
	m_device.MouseMovedEvent.RemoveListener(m_mouseMovedListener);
}

OnyxEditor::Inputs::EKeyState OnyxEditor::Inputs::InputManager::GetKeyState(EKey p_key) const
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

OnyxEditor::Inputs::EMouseButtonState OnyxEditor::Inputs::InputManager::GetMouseButtonState(EMouseButton p_button) const
{
	uint32_t state = SDL_GetMouseState(nullptr, nullptr);

	if (state & SDL_BUTTON(static_cast<int>(p_button)))
	{
		return EMouseButtonState::MOUSE_DOWN;
	}

	return EMouseButtonState::MOUSE_UP;
}

bool OnyxEditor::Inputs::InputManager::IsKeyPressed(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_DOWN;
}

bool OnyxEditor::Inputs::InputManager::IsKeyReleased(EKey p_key) const
{
	return m_keyEvents.find(p_key) != m_keyEvents.end() && m_keyEvents.at(p_key) == EKeyState::KEY_UP;
}

bool OnyxEditor::Inputs::InputManager::IsMouseButtonPressed(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_DOWN;
}

bool OnyxEditor::Inputs::InputManager::IsMouseButtonReleased(EMouseButton p_button) const
{
	return m_mouseButtonEvents.find(p_button) != m_mouseButtonEvents.end() && m_mouseButtonEvents.at(p_button) == EMouseButtonState::MOUSE_UP;
}

std::pair<int, int> OnyxEditor::Inputs::InputManager::GetMousePosition() const
{
	std::pair<int, int> result;
	SDL_GetMouseState(&result.first, &result.second);
	return result;
}

std::pair<int, int> OnyxEditor::Inputs::InputManager::GetMouseRelativeMovement() const
{
	return m_mouseRelativeMovement;
}

int OnyxEditor::Inputs::InputManager::GetMouseWheel() const
{
	return m_mouseWheel;
}

void OnyxEditor::Inputs::InputManager::LockMouse() const
{
	m_device.SetRelativeMouseMode(true);
}

void OnyxEditor::Inputs::InputManager::UnlockMouse() const
{
	m_device.SetRelativeMouseMode(false);
}

bool OnyxEditor::Inputs::InputManager::IsMouseLocked() const
{
	return m_device.GetRelativeMouseMode();
}

void OnyxEditor::Inputs::InputManager::ClearEvents()
{
	m_keyEvents.clear();
	m_mouseButtonEvents.clear();
}

void OnyxEditor::Inputs::InputManager::OnKeyPressed(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_DOWN;
}

void OnyxEditor::Inputs::InputManager::OnKeyReleased(int p_key)
{
	m_keyEvents[static_cast<EKey>(p_key)] = EKeyState::KEY_UP;
}

void OnyxEditor::Inputs::InputManager::OnMouseButtonPressed(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_DOWN;
}

void OnyxEditor::Inputs::InputManager::OnMouseButtonReleased(int p_button)
{
	m_mouseButtonEvents[static_cast<EMouseButton>(p_button)] = EMouseButtonState::MOUSE_UP;
}

void OnyxEditor::Inputs::InputManager::OnMouseMoved(std::pair<int, int> p_motion)
{
	m_mouseRelativeMovement = p_motion;
}

void OnyxEditor::Inputs::InputManager::OnMouseWheel(int p_wheel)
{
	m_mouseWheel = p_wheel;
}
