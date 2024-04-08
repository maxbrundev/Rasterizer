#pragma once

#include "Context/Window.h"

#include "Inputs/InputManager.h"

#include "Entities/Camera.h"

namespace Rendering
{
	class CameraController
	{
		enum class ECameraDirection
		{
			FORWARD,
			BACKWARD,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

	public:
		CameraController(Entities::Camera& p_camera,  glm::vec3& p_position);
		~CameraController() = default;

		void Update(float p_deltaTime);
		
		void SetPosition(const glm::vec3& p_position);
		void SetPosition(float p_posX, float p_posY, float p_posZ);
		
		const glm::vec3& GetPosition() const;

	private:
		void ProcessKeyboard(ECameraDirection p_direction, float p_deltaTime);
		void ProcessMouseMovement(float p_offsetX, float p_offsetY);
		void HandleInputs(float p_deltaTime);
		void HandleMouse(float p_deltaTime);
		void HandleCameraZoom();

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