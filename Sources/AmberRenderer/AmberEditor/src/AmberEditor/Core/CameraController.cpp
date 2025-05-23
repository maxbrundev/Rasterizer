#include "AmberEditor/Core/CameraController.h"

#include "AmberEditor/Tools/Globals/ServiceLocator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

AmberEditor::Core::CameraController::CameraController(Entities::Camera& p_camera, glm::vec3& p_position, glm::quat& p_rotation) :
	m_window(Tools::Globals::ServiceLocator::Get<Context::Window>()),
	m_inputManager(Tools::Globals::ServiceLocator::Get<Inputs::InputManager>()),
	m_camera(p_camera),
	m_position(p_position),
	m_rotation(p_rotation),
	m_targetPosition{},
	m_currentMovement{}
{
}

void AmberEditor::Core::CameraController::Update(float p_deltaTime)
{
	HandleInputs(p_deltaTime);
	HandleMouse();
	HandleCameraZoom();

	if (m_inputManager.IsMouseButtonPressed(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_inputManager.LockMouse();
		m_window.SetCursorMode(Context::Settings::ECursorMode::DISABLED);
	}
	else if (m_inputManager.IsMouseButtonReleased(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_isFirstMouse = true;
		m_rightMousePressed = false;
		m_window.SetCursorMode(Context::Settings::ECursorMode::NORMAL);
		m_inputManager.UnlockMouse();
	}

	if (m_inputManager.IsMouseButtonPressed(Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_middleMousePressed = true;
		m_window.SetCursorMode(Context::Settings::ECursorMode::DISABLED);
		m_inputManager.LockMouse();
	}
	else if (m_inputManager.IsMouseButtonReleased(Inputs::EMouseButton::MOUSE_BUTTON_MIDDLE))
	{
		m_middleMousePressed = false;
		m_window.SetCursorMode(Context::Settings::ECursorMode::NORMAL);
		m_inputManager.UnlockMouse();
	}
}

glm::vec3 RemoveRoll(const glm::vec3& p_eulerRotation)
{
	glm::vec3 result = p_eulerRotation;

	if (result.z >= 179.0f || result.z <= -179.0f)
	{
		result.x += result.z;
		result.y = 180.0f - result.y;
		result.z = 0.0f;
	}

	if (result.x > 180.0f)  result.x -= 360.0f;
	if (result.x < -180.0f) result.x += 360.0f;

	return result;
}


void AmberEditor::Core::CameraController::HandleMouse()
{
	if (m_rightMousePressed || m_middleMousePressed)
	{
		const auto [xPos, yPos] = m_inputManager.GetMouseRelativeMovement();

		if (m_isFirstMouse)
		{
			m_eulerRotation = glm::degrees(glm::eulerAngles(m_rotation));

			m_eulerRotation = RemoveRoll(m_eulerRotation);

			m_isFirstMouse = false;
		}

		
		const glm::vec2 mouseOffset
		{
			static_cast<float>(xPos),
			static_cast<float>(yPos)
		};

		
		if (m_rightMousePressed)
		{
			auto mouseDelta = mouseOffset * m_mouseSensitivity;

			m_eulerRotation.y -= mouseDelta.x;
			m_eulerRotation.x += mouseDelta.y;
			m_eulerRotation.x = std::max(std::min(m_eulerRotation.x, 90.0f), -90.0f);

			m_rotation = glm::qua(glm::radians(m_eulerRotation));
		}

		if (m_middleMousePressed)
		{
			auto mouseDelta = mouseOffset * 0.01f;

			const glm::vec3 right = m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
			const glm::vec3 up = m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);

			const glm::vec3 delta = right * mouseDelta.x + up * mouseDelta.y;

			m_position += delta * m_moveSpeed;
		}
	}
}

void AmberEditor::Core::CameraController::SetPosition(const glm::vec3& p_position) const
{
	m_position = p_position;
}

void AmberEditor::Core::CameraController::SetPosition(float p_posX, float p_posY, float p_posZ) const
{
	m_position.x = p_posX;
	m_position.y = p_posY;
	m_position.z = p_posZ;
}

const glm::vec3& AmberEditor::Core::CameraController::GetPosition() const
{
	return m_position;
}

bool AmberEditor::Core::CameraController::IsRightMousePressed() const
{
	return m_rightMousePressed;
}

void AmberEditor::Core::CameraController::SetRotation(const glm::quat& p_rotation) const
{
	m_rotation = p_rotation;
}

const glm::quat& AmberEditor::Core::CameraController::GetRotation() const
{
	return m_rotation;
}

void AmberEditor::Core::CameraController::SetSpeed(float p_value)
{
	m_moveSpeed = p_value;
}

float AmberEditor::Core::CameraController::GetSpeed() const
{
	return m_moveSpeed;
}

void AmberEditor::Core::CameraController::HandleInputs(float p_deltaTime)
{
	m_targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	if (m_rightMousePressed)
	{
		bool run = m_inputManager.GetKeyState(Inputs::EKey::KEY_LSHIFT) == Inputs::EKeyState::KEY_DOWN;

		float velocity = m_moveSpeed * p_deltaTime * (run ? 2.0f : 1.0f);

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_W) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_rotation * glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_A) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_rotation * glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_S) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_rotation * glm::vec3(0.0f, 0.0f, 1.0f) * -velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_D) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_rotation * glm::vec3(1.0f, 0.0f, 0.0f) * -velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_E) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += glm::vec3(0.0f, velocity, 0.0f);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_Q) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += glm::vec3(0.0f, -velocity, 0.0f);
		}
	}

	m_currentMovement = glm::lerp(m_currentMovement, m_targetPosition, 10.0f * p_deltaTime);
	m_position += m_currentMovement;
}

void AmberEditor::Core::CameraController::HandleCameraZoom() const
{
	m_position += m_rotation * glm::vec3(0.0f, 0.0f, 1.0f) * static_cast<float>(m_inputManager.GetMouseWheel());
}
