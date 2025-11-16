#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "OnyxEditor/Resources/Model.h"

namespace OnyxEditor::Controllers
{
	class PlaneController
	{
	public:
		PlaneController(Resources::Model* p_model);
		~PlaneController() = default;

		void Update(float p_deltaTime);

		const glm::vec3& GetPosition() const { return m_position; }
		const glm::quat& GetRotation() const { return m_currentRotation; }
		const glm::vec3& GetForward() const { return m_forward; }
		float GetYaw() const { return m_yaw; }
		Resources::Model* GetModel() const { return m_model; }
		Resources::Model* m_model;
	private:
		void HandleInput(float p_deltaTime);
		void UpdateTransform();

	private:
		const float SPEED = 10.0f;
		const float ROLL_SPEED = glm::pi<float>() / 2.0f;
		const float PITCH_SPEED = glm::pi<float>() / 3.5f;
		const float PITCH_MAX = glm::radians(30.0f);
		const float ROLL_MAX = glm::radians(60.0f);

		glm::quat m_currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_position = glm::vec3(0.0f, 10.0f, 0.0f);
		glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, 1.0f);

		float m_pitch = 0.0f;
		float m_yaw = 0.0f;
		float m_roll = 0.0f;

		float m_targetPitch = 0.0f;
		float m_targetYaw = 0.0f;
		float m_targetRoll = 0.0f;
	};
}
