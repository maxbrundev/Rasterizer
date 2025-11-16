#include "OnyxEditor/Entities/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

OnyxEditor::Entities::Camera::Camera() :
m_viewMatrix(1.0f),
m_projectionMatrix(1.0f),
m_clearColor(0.1f, 0.1f, 0.1f),
m_fov(60.0f),
m_size(5.0f),
m_near(0.1f),
m_far(1000.0f),
m_clearColorBuffer(true),
m_clearDepthBuffer(true),
m_projectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE)
{
	SetClearColor({ 0.275f, 0.275f, 0.275f });
}

void OnyxEditor::Entities::Camera::ComputeMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight, const glm::vec3& p_position, const glm::quat& p_rotation)
{
	ComputeViewMatrix(p_position, p_rotation);
	ComputeProjectionMatrix(p_windowWidth, p_windowHeight);
}

void OnyxEditor::Entities::Camera::SetProjectionMode(Rendering::Settings::EProjectionMode p_projectionMode)
{
	m_projectionMode = p_projectionMode;
}

void OnyxEditor::Entities::Camera::SetSize(float p_value)
{
	m_size = p_value;
}

glm::mat4& OnyxEditor::Entities::Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

glm::mat4& OnyxEditor::Entities::Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

void OnyxEditor::Entities::Camera::SetFov(float p_value)
{
	m_fov = p_value;
}

void OnyxEditor::Entities::Camera::SetNear(float p_value)
{
	m_near = p_value;
}

void OnyxEditor::Entities::Camera::SetFar(float p_value)
{
	m_far = p_value;
}

void OnyxEditor::Entities::Camera::SetClearColorBuffer(bool p_value)
{
	m_clearColorBuffer = p_value;
}

void OnyxEditor::Entities::Camera::SetClearDepthBuffer(bool p_value)
{
	m_clearDepthBuffer = p_value;
}

bool OnyxEditor::Entities::Camera::GetClearColorBuffer() const
{
	return m_clearColorBuffer;
}

bool OnyxEditor::Entities::Camera::GetClearDepthBuffer() const
{
	return m_clearDepthBuffer;
}

void OnyxEditor::Entities::Camera::SetClearColor(const glm::vec3& p_clearColor)
{
	m_clearColor = p_clearColor;
}

OnyxEditor::Rendering::Settings::EProjectionMode OnyxEditor::Entities::Camera::GetProjectionMode() const
{
	return m_projectionMode;
}

float OnyxEditor::Entities::Camera::GetFov() const
{
	return m_fov;
}

float OnyxEditor::Entities::Camera::GetSize() const
{
	return m_size;
}

float OnyxEditor::Entities::Camera::GetNear() const
{
	return m_near;
}

float OnyxEditor::Entities::Camera::GetFar() const
{
	return m_far;
}

const glm::vec3& OnyxEditor::Entities::Camera::GetClearColor() const
{
	return m_clearColor;
}

void OnyxEditor::Entities::Camera::ComputeViewMatrix(const glm::vec3& p_position, const glm::quat& p_rotation)
{
	const auto& up = p_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	const auto& forward = p_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	m_viewMatrix = glm::lookAt(p_position, p_position + forward, up);
}

void OnyxEditor::Entities::Camera::ComputeProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight)
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