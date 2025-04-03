#pragma once

#include "AmberEditor/Context/Window.h"

#include "AmberEditor/Inputs/InputManager.h"

#include "AmberEditor/Entities/Camera.h"

namespace AmberEditor::Core
{
	class CameraController
	{
	public:
		CameraController(Entities::Camera& p_camera, glm::vec3& p_position, glm::quat& p_rotation);
		~CameraController() = default;

		void Update(float p_deltaTime);

		void SetPosition(const glm::vec3& p_position) const;
		void SetPosition(float p_posX, float p_posY, float p_posZ) const;

		void SetRotation(const glm::quat& p_rotation) const;
		const glm::quat& GetRotation() const;
		void SetSpeed(float p_value);
		float GetSpeed() const;
		const glm::vec3& GetPosition() const;
		bool IsRightMousePressed() const;

	private:
		void HandleInputs(float p_deltaTime);
		void HandleMouse();
		void HandleCameraZoom() const;

	private:
		Context::Window& m_window;
		Inputs::InputManager& m_inputManager;

		Entities::Camera& m_camera;
		glm::vec3 m_eulerRotation;
		glm::vec3& m_position;
		glm::quat& m_rotation;

		float m_mouseSensitivity = 0.9f;
		float m_moveSpeed = 15.0f;

		bool m_isFirstMouse = true;
		bool m_rightMousePressed = false;
		bool m_middleMousePressed = false;

		glm::vec3 m_targetPosition;
		glm::vec3 m_currentMovement;
	};
}