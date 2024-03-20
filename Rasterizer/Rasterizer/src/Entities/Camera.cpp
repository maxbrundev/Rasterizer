#include "Entities/Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


Entities::Camera::Camera() : m_clearColor(0.1f, 0.1f, 0.1f), m_yaw(-90.0f), m_pitch(0.0f), m_fov(45.0f), m_near(0.1f), m_far(100.0f)
{
	UpdateCameraVectors();
}

void Entities::Camera::UpdateCameraVectors()
{
	glm::vec3 front;

	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_forward = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	m_up = glm::normalize(glm::cross(m_right, m_forward));
}

void Entities::Camera::CalculateMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const glm::vec3& p_position)
{
	CalculateViewMatrix(p_position, m_up);
	CalculateProjectionMatrix(p_windowWidth, p_windowHeight);
}

glm::mat4& Entities::Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

glm::mat4& Entities::Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

const glm::vec3& Entities::Camera::GetForward() const
{
	return m_forward;
}

const glm::vec3& Entities::Camera::GetRight() const
{
	return m_right;
}

const glm::vec3& Entities::Camera::GetUp() const
{
	return m_up;
}

void Entities::Camera::SetFov(float p_value)
{
	m_fov = p_value;
}

void Entities::Camera::SetNear(float p_value)
{
	m_near = p_value;
}

void Entities::Camera::SetFar(float p_value)
{
	m_far = p_value;
}

void Entities::Camera::SetClearColor(const glm::vec3& p_clearColor)
{
	m_clearColor = p_clearColor;
}

float& Entities::Camera::GetCameraFov()
{
	return m_fov;
}

float& Entities::Camera::GetYaw()
{
	return m_yaw;
}

float& Entities::Camera::GetPitch()
{
	return m_pitch;
}

const glm::vec3& Entities::Camera::GetClearColor() const
{
	return m_clearColor;
}

void Entities::Camera::CalculateViewMatrix(const glm::vec3& p_position, const  glm::vec3& p_up)
{
	m_viewMatrix = glm::lookAt(p_position, p_position + m_forward, p_up);
}

void Entities::Camera::CalculateProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight)
{
	if(p_windowHeight > 0)
	{
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), p_windowWidth / static_cast<float>(p_windowHeight), m_near, m_far);
	}
}
