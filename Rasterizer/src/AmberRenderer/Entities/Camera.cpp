#include "AmberRenderer/Entities/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


AmberRenderer::Entities::Camera::Camera() :
m_viewMatrix(1.0f),
m_projectionMatrix(1.0f),
m_clearColor(0.1f, 0.1f, 0.1f),
m_fov(60.0f), 
m_size(5.0f),
m_near(0.1f),
m_far(100.0f),
m_projectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE)
{
}

void AmberRenderer::Entities::Camera::ComputeMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const glm::vec3& p_position, const glm::quat& p_rotation)
{
	ComputeViewMatrix(p_position, p_rotation);
	ComputeProjectionMatrix(p_windowWidth, p_windowHeight);
}

void AmberRenderer::Entities::Camera::SetProjectionMode(Rendering::Settings::EProjectionMode p_projectionMode)
{
	m_projectionMode = p_projectionMode;
}

void AmberRenderer::Entities::Camera::SetSize(float p_value)
{
	m_size = p_value;
}

glm::mat4& AmberRenderer::Entities::Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

glm::mat4& AmberRenderer::Entities::Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

void AmberRenderer::Entities::Camera::SetFov(float p_value)
{
	m_fov = p_value;
}

void AmberRenderer::Entities::Camera::SetNear(float p_value)
{
	m_near = p_value;
}

void AmberRenderer::Entities::Camera::SetFar(float p_value)
{
	m_far = p_value;
}

void AmberRenderer::Entities::Camera::SetClearColor(const glm::vec3& p_clearColor)
{
	m_clearColor = p_clearColor;
}

AmberRenderer::Rendering::Settings::EProjectionMode AmberRenderer::Entities::Camera::GetProjectionMode() const
{
	return m_projectionMode;
}

float AmberRenderer::Entities::Camera::GetFov() const
{
	return m_fov;
}

float AmberRenderer::Entities::Camera::GetSize() const
{
	return m_size;
}

float AmberRenderer::Entities::Camera::GetNear() const
{
	return m_near;
}

float AmberRenderer::Entities::Camera::GetFar() const
{
	return m_far;
}

const glm::vec3& AmberRenderer::Entities::Camera::GetClearColor() const
{
	return m_clearColor;
}

void AmberRenderer::Entities::Camera::ComputeViewMatrix(const glm::vec3& p_position, const glm::quat& p_rotation)
{
	const auto& up = p_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	const auto& forward = p_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	m_viewMatrix = glm::lookAt(p_position, p_position + forward, up);
}

void AmberRenderer::Entities::Camera::ComputeProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight)
{
	const auto ratio = p_windowWidth / static_cast<float>(p_windowHeight);

	switch (m_projectionMode)
	{
	case Rendering::Settings::EProjectionMode::PERSPECTIVE:
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), ratio, m_near, m_far);
		break;
	case Rendering::Settings::EProjectionMode::ORTHOGRAPHIC:
	{
		const auto right = m_size * ratio;
		const auto left = -right;

		const auto top = m_size;
		const auto bottom = -top;
		m_projectionMatrix = glm::ortho(left, right, bottom, top, m_near, m_far);
	}
	break;
	}
}