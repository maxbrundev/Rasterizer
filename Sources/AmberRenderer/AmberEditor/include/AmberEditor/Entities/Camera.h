#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "AmberEditor/Rendering/Settings/EProjectionMode.h"

namespace AmberEditor::Entities
{
	class Camera
	{
	public:
		Camera();
		~Camera() = default;

		void ComputeMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const glm::vec3& p_position, const glm::quat& p_rotation);

		void SetProjectionMode(Rendering::Settings::EProjectionMode p_projectionMode);

		void SetSize(float p_value);
		void SetFov(float p_value);
		void SetNear(float p_value);
		void SetFar(float p_value);

		void SetClearColorBuffer(bool p_value);
		void SetClearDepthBuffer(bool p_value);

		void SetClearColor(const glm::vec3& p_clearColor);

		Rendering::Settings::EProjectionMode GetProjectionMode() const;

		bool GetClearColorBuffer() const;
		bool GetClearDepthBuffer() const;

		float GetFov() const;
		float GetSize() const;
		float GetNear() const;
		float GetFar() const;

		const glm::vec3& GetClearColor() const;

		glm::mat4& GetViewMatrix();
		glm::mat4& GetProjectionMatrix();

	private:
		void ComputeViewMatrix(const glm::vec3& p_position, const glm::quat& p_rotation);
		void ComputeProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight);

	private:
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;

		glm::vec3 m_clearColor;

		float m_fov;
		float m_size;
		float m_near;
		float m_far;

		bool m_clearColorBuffer;
		bool m_clearDepthBuffer;

		Rendering::Settings::EProjectionMode m_projectionMode;
	};
}
