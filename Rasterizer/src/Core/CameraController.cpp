#include "Core/CameraController.h"

#include "Tools/Globals/ServiceLocator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

Core::CameraController::CameraController(Entities::Camera& p_camera, glm::vec3& p_position) :
	m_window(Tools::Globals::ServiceLocator::Get<Context::Window>()),
	m_inputManager(Tools::Globals::ServiceLocator::Get<Inputs::InputManager>()),
	m_camera(p_camera),
	m_position(p_position),
	m_targetPosition{},
	m_currentMovement{}
{
}

void Core::CameraController::Update(float p_deltaTime)
{
	HandleFPSInputs(p_deltaTime);
	HandleFPSMouse(p_deltaTime);
	HandleZoom();

	if (m_inputManager.IsMouseButtonPressed(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = true;
		m_inputManager.LockMouse();
		m_window.SetCursorMode(Context::Settings::ECursorMode::DISABLED);
	}
	else if (m_inputManager.IsMouseButtonReleased(Inputs::EMouseButton::MOUSE_BUTTON_RIGHT))
	{
		m_rightMousePressed = false;
		m_inputManager.UnlockMouse();
		m_window.SetCursorMode(Context::Settings::ECursorMode::NORMAL);
	}
}

void Core::CameraController::ProcessMouseMovement(float p_offsetX, float p_offsetY) const
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

void Core::CameraController::SetPosition(const glm::vec3& p_position) const
{
	m_position = p_position;
}

void Core::CameraController::SetPosition(float p_posX, float p_posY, float p_posZ) const
{
	m_position.x = p_posX;
	m_position.y = p_posY;
	m_position.z = p_posZ;
}

const glm::vec3& Core::CameraController::GetPosition() const
{
	return m_position;
}

void Core::CameraController::HandleFPSInputs(float p_deltaTime)
{
	m_targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	float velocity = m_moveSpeed * p_deltaTime;

	if (m_rightMousePressed) 
	{
		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_W) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_camera.GetForward() * velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_A) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_camera.GetRight() * -velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_S) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_camera.GetForward() * -velocity;
		}

		if (m_inputManager.GetKeyState(Inputs::EKey::KEY_D) == Inputs::EKeyState::KEY_DOWN)
		{
			m_targetPosition += m_camera.GetRight() * velocity;
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

void Core::CameraController::HandleFPSMouse(float p_deltaTime) const
{
	if (m_rightMousePressed)
	{
		auto[xMovement, yMovement] = m_inputManager.GetMouseRelativeMovement();

		const double xOffset = xMovement * m_mouseSensitivity * p_deltaTime;
		const double yOffset = yMovement * m_mouseSensitivity * p_deltaTime;

		ProcessMouseMovement(xOffset, yOffset);
	}
}

void Core::CameraController::HandleZoom() const
{
	m_position += m_camera.GetForward() * static_cast<float>(m_inputManager.GetMouseWheel());
}
