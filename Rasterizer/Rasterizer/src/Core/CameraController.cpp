#include "Core/CameraController.h"

#include "Tools/Globals/ServiceLocator.h"
#include <glm/gtx/compatibility.hpp>

Rendering::CameraController::CameraController(Entities::Camera& p_camera, glm::vec3& p_position) :
	m_window(Tools::Globals::ServiceLocator::Get<Context::Window>()),
	m_inputManager(Tools::Globals::ServiceLocator::Get<Inputs::InputManager>()),
	m_camera(p_camera), 
	m_position(p_position)
{
	m_camera.SetFov(60.0f);
}

void Rendering::CameraController::Update(float p_deltaTime)
{
	HandleInputs(p_deltaTime);
	HandleMouse(p_deltaTime);
	HandleCameraZoom();

	if (m_inputManager.IsMouseButtonPressed(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_inputManager.LockMouse();
		m_window.SetCursorMode(Context::ECursorMode::DISABLED);
	}
	else if (m_inputManager.IsMouseButtonReleased(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = false;
		m_inputManager.UnlockMouse();
		m_window.SetCursorMode(Context::ECursorMode::NORMAL);
	}
}

void Rendering::CameraController::ProcessKeyboard(ECameraDirection p_direction, float p_deltaTime)
{
	const glm::vec3 forward = m_camera.GetForward();
	const glm::vec3 right   = m_camera.GetRight();
	const glm::vec3 up      = m_camera.GetUp();

	const float velocity = m_moveSpeed * p_deltaTime;

	switch (p_direction)
	{
	case ECameraDirection::FORWARD:
		m_targetPosition += forward;
		break;
	case ECameraDirection::BACKWARD:
		m_targetPosition -= forward;
		break;
	case ECameraDirection::LEFT:
		m_targetPosition -= right;
		break;
	case ECameraDirection::RIGHT:
		m_targetPosition += right;
		break;
	case ECameraDirection::UP:
		m_targetPosition += up;
		break;
	case ECameraDirection::DOWN:
		m_targetPosition -= up;
		break;
	}

	m_targetPosition *= velocity;
}

void Rendering::CameraController::ProcessMouseMovement(float p_offsetX, float p_offsetY)
{
	float& yaw   = m_camera.GetYaw();
	float& pitch = m_camera.GetPitch();

	yaw   += p_offsetX;
	pitch -= p_offsetY;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	m_camera.UpdateCameraVectors();
}

void Rendering::CameraController::SetPosition(const glm::vec3& p_position)
{
	m_position = p_position;
}

void Rendering::CameraController::SetPosition(float p_posX, float p_posY, float p_posZ)
{
	m_position.x = p_posX;
	m_position.y = p_posY;
	m_position.z = p_posZ;
}

const glm::vec3& Rendering::CameraController::GetPosition() const
{
	return m_position;
}

void Rendering::CameraController::HandleInputs(float p_deltaTime)
{
	m_targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	if (m_rightMousePressed) 
	{
		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_W) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::FORWARD, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_S) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::BACKWARD, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_A) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::LEFT, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_D) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::RIGHT, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_E) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::UP, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_Q) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(ECameraDirection::DOWN, p_deltaTime);
		}
	}

	m_currentMovement = glm::lerp(m_currentMovement, m_targetPosition, 10.0f * p_deltaTime);
	m_position += m_currentMovement;
}

void Rendering::CameraController::HandleMouse(float p_deltaTime)
{
	if (m_rightMousePressed)
	{
		auto[xMovement, yMovement] = m_inputManager.GetMouseRelativeMovement();

		const double xOffset = xMovement * m_mouseSensitivity * p_deltaTime;
		const double yOffset = yMovement * m_mouseSensitivity * p_deltaTime;

		ProcessMouseMovement(xOffset, yOffset);
	}
}

void Rendering::CameraController::HandleCameraZoom()
{
	m_position += m_camera.GetForward() * static_cast<float>(m_inputManager.GetMouseWheel());
}