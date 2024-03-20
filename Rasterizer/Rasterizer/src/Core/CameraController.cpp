#include "Core/CameraController.h"

#include "Tools/Globals/ServiceLocator.h"
#include <glm/gtx/compatibility.hpp>

Core::CameraController::CameraController(Entities::Camera& p_camera, glm::vec3& p_position) :
	m_window(Tools::Globals::ServiceLocator::Get<Context::Window>()),
	m_inputManager(Tools::Globals::ServiceLocator::Get<Inputs::InputManager>()),
	m_camera(p_camera), 
	m_position(p_position)
{
	m_camera.SetFov(60.0f);
}

void Core::CameraController::ProcessKeyboard(cameraMovement p_direction, float p_deltaTime)
{
	const glm::vec3 forward = m_camera.GetForward();
	const glm::vec3 right = m_camera.GetRight();
	const glm::vec3 up = m_camera.GetUp();

	const float velocity = m_moveSpeed * p_deltaTime;

	if (p_direction == cameraMovement::FORWARD)
	{
		m_targetPosition += forward * velocity;
	}
	if (p_direction == cameraMovement::BACKWARD)
	{
		m_targetPosition -= forward * velocity;
	}
	if (p_direction == cameraMovement::LEFT)
	{
		m_targetPosition -= right * velocity;
	}
	if (p_direction == cameraMovement::RIGHT)
	{
		m_targetPosition += right * velocity;
	}
	if (p_direction == cameraMovement::UP)
	{
		m_targetPosition += up * velocity;
	}
	if (p_direction == cameraMovement::DOWN)
	{
		m_targetPosition -= up * velocity;
	}
}

void Core::CameraController::ProcessMouseMovement(float p_offsetX, float p_offsetY)
{
	if (m_rightMousePressed) 
	{
		float& yaw = m_camera.GetYaw();
		float& pitch = m_camera.GetPitch();

		p_offsetX *= m_mouseSensitivity;
		p_offsetY *= m_mouseSensitivity;

		yaw += p_offsetX;
		pitch += p_offsetY;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}

	m_camera.UpdateCameraVectors();
}

void Core::CameraController::ProcessMouseScroll(float p_offsetY)
{
	float& fov = m_camera.GetCameraFov();

	if (fov >= 1.0f && fov <= 45.0f)
	{
		fov -= p_offsetY;
	}

	if (fov <= 1.0f)
	{
		fov = 1.0f;
	}

	if (fov >= 45.0f)
	{
		fov = 45.0f;
	}
}

void Core::CameraController::Update(float p_deltaTime)
{
	HandleInputs(p_deltaTime);
	HandleMouse();
	HandleCameraZoom();

	if (m_inputManager.IsMouseButtonPressed(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_window.SetCursorMode(Context::ECursorMode::DISABLED);
	}

	if (m_inputManager.IsMouseButtonReleased(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_isFirstMouse = true;
		m_rightMousePressed = false;
		m_window.SetCursorMode(Context::ECursorMode::NORMAL);
	}
}

void Core::CameraController::SetPosition(const glm::vec3& p_position)
{
	m_position = p_position;
}

void Core::CameraController::SetPosition(float p_posX, float p_posY, float p_posZ)
{
	m_position.x = p_posX;
	m_position.y = p_posY;
	m_position.z = p_posZ;
}

const glm::vec3& Core::CameraController::GetPosition() const
{
	return m_position;
}

void Core::CameraController::HandleInputs(float p_deltaTime)
{
	m_targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	if (m_rightMousePressed) 
	{
		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_W) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::FORWARD, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_S) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::BACKWARD, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_A) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::LEFT, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_D) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::RIGHT, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_E) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::UP, p_deltaTime);
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_Q) == Inputs::EKeyState::KEY_DOWN)
		{
			ProcessKeyboard(cameraMovement::DOWN, p_deltaTime);
		}
	}

	m_currentMovement = glm::lerp(m_currentMovement, m_targetPosition, 10.0f * p_deltaTime);
	m_position += m_currentMovement;
}

void Core::CameraController::HandleMouse()
{
	if (m_rightMousePressed)
	{
		auto[xPos, yPos] = m_inputManager.GetMousePosition();

		if (m_isFirstMouse)
		{
			m_lastMousePosX = xPos;
			m_lastMousePosY = yPos;
			m_isFirstMouse = false;
		}

		const double offsetX = xPos - m_lastMousePosX;
		const double offsetY = m_lastMousePosY - yPos;

		m_lastMousePosX = xPos;
		m_lastMousePosY = yPos;

		ProcessMouseMovement(offsetX, offsetY);
	}
}

void Core::CameraController::HandleCameraZoom()
{
	//m_position += m_camera.GetForward();
}
