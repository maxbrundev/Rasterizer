#pragma once

#include "Context/Window.h"

#include "Inputs/InputManager.h"

#include "Entities/Camera.h"

namespace Core
{
	class CameraController
	{
	public:
		CameraController(Entities::Camera& p_camera,  glm::vec3& p_position);
		~CameraController() = default;

		void Update(float p_deltaTime);
		
		void SetPosition(const glm::vec3& p_position) const;
		void SetPosition(float p_posX, float p_posY, float p_posZ) const;
		
		const glm::vec3& GetPosition() const;

	private:
		void ProcessMouseMovement(float p_offsetX, float p_offsetY) const;
		void HandleFPSInputs(float p_deltaTime);
		void HandleFPSMouse(float p_deltaTime) const;
		void HandleZoom() const;

	private:
		Context::Window& m_window;
		Inputs::InputManager& m_inputManager;

		Entities::Camera& m_camera;

		glm::vec3& m_position;

		float m_mouseSensitivity = 10.0f;
		float m_moveSpeed = 10.0f;

		bool m_rightMousePressed = false;

		glm::vec3 m_targetPosition;
		glm::vec3 m_currentMovement;
	};
}