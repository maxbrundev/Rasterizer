#include "OnyxEditor/Maths/Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

OnyxEditor::Maths::Transform::Transform(glm::vec3 p_localPosition, glm::quat p_localRotation, glm::vec3 p_localScale) :
m_parent(nullptr),
m_notificationHandlerID(-1)
{
	GenerateMatricesLocal(p_localPosition, p_localRotation, p_localScale);
}

OnyxEditor::Maths::Transform::Transform(const Transform& p_other) : Transform(p_other.m_worldPosition, p_other.m_worldRotation, p_other.m_worldScale)
{
}

OnyxEditor::Maths::Transform::~Transform()
{
	m_notifier.NotifyChildren(TransformNotifier::ENotification::TRANSFORM_DESTROYED);
}

OnyxEditor::Maths::Transform& OnyxEditor::Maths::Transform::operator=(const Transform& p_other)
{
	GenerateMatricesWorld(p_other.m_worldPosition, p_other.m_worldRotation, p_other.m_worldScale);

	return *this;
}

void OnyxEditor::Maths::Transform::NotificationHandler(TransformNotifier::ENotification p_notification)
{
	switch (p_notification)
	{
	case TransformNotifier::ENotification::TRANSFORM_CHANGED:
		UpdateWorldMatrix();
		break;

	case TransformNotifier::ENotification::TRANSFORM_DESTROYED:
		GenerateMatricesLocal(m_worldPosition, m_worldRotation, m_worldScale);
		m_parent = nullptr;
		UpdateWorldMatrix();
		break;
	}
}

void OnyxEditor::Maths::Transform::SetParent(Transform& p_parent)
{
	m_parent = &p_parent;

	m_notificationHandlerID = m_parent->m_notifier.AddNotificationHandler(std::bind(&Transform::NotificationHandler, this, std::placeholders::_1));

	UpdateWorldMatrix();
}

bool OnyxEditor::Maths::Transform::RemoveParent()
{
	if (m_parent != nullptr)
	{
		m_parent->m_notifier.RemoveNotificationHandler(m_notificationHandlerID);
		m_parent = nullptr;
		UpdateWorldMatrix();

		return true;
	}

	return false;
}

bool OnyxEditor::Maths::Transform::HasParent() const
{
	return m_parent != nullptr;
}

void OnyxEditor::Maths::Transform::GenerateMatricesLocal(glm::vec3 p_position, glm::quat p_rotation, glm::vec3 p_scale)
{
	m_localMatrix = glm::translate(glm::mat4(1.0f), p_position) * glm::mat4_cast(glm::normalize(p_rotation)) * glm::scale(glm::mat4(1.0f), p_scale);
	m_localPosition = p_position;
	m_localRotation = p_rotation;
	m_localScale = p_scale;

	UpdateWorldMatrix();
}

void OnyxEditor::Maths::Transform::GenerateMatricesWorld(glm::vec3 p_position, glm::quat p_rotation, glm::vec3 p_scale)
{
	m_worldMatrix = glm::translate(glm::mat4(1.0f), p_position) * glm::mat4_cast(glm::normalize(p_rotation)) * glm::scale(glm::mat4(1.0f), p_scale);
	m_worldPosition = p_position;
	m_worldRotation = p_rotation;
	m_worldScale = p_scale;

	UpdateLocalMatrix();
}

void OnyxEditor::Maths::Transform::UpdateWorldMatrix()
{
	m_worldMatrix = HasParent() ? m_parent->m_worldMatrix * m_localMatrix : m_localMatrix;
	PreDecomposeWorldMatrix();

	m_notifier.NotifyChildren(TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void OnyxEditor::Maths::Transform::UpdateLocalMatrix()
{
	m_localMatrix = HasParent() ? glm::inverse(m_parent->m_worldMatrix) * m_worldMatrix : m_worldMatrix;
	PreDecomposeLocalMatrix();

	m_notifier.NotifyChildren(TransformNotifier::ENotification::TRANSFORM_CHANGED);
}

void OnyxEditor::Maths::Transform::SetLocalPosition(glm::vec3 p_newPosition)
{
	GenerateMatricesLocal(p_newPosition, m_localRotation, m_localScale);
}

void OnyxEditor::Maths::Transform::SetLocalRotation(glm::quat p_newRotation)
{
	GenerateMatricesLocal(m_localPosition, p_newRotation, m_localScale);
}

void OnyxEditor::Maths::Transform::SetLocalScale(glm::vec3 p_newScale)
{
	GenerateMatricesLocal(m_localPosition, m_localRotation, p_newScale);
}

void OnyxEditor::Maths::Transform::SetWorldPosition(glm::vec3 p_newPosition)
{
	GenerateMatricesWorld(p_newPosition, m_worldRotation, m_worldScale);
}

void OnyxEditor::Maths::Transform::SetWorldRotation(glm::quat p_newRotation)
{
	GenerateMatricesWorld(m_worldPosition, p_newRotation, m_worldScale);
}

void OnyxEditor::Maths::Transform::SetWorldScale(glm::vec3 p_newScale)
{
	GenerateMatricesWorld(m_worldPosition, m_worldRotation, p_newScale);
}

void OnyxEditor::Maths::Transform::TranslateLocal(const glm::vec3& p_translation)
{
	SetLocalPosition(m_localPosition + p_translation);
}

void OnyxEditor::Maths::Transform::RotateLocal(const glm::quat& p_rotation)
{
	SetLocalRotation(m_localRotation * p_rotation);
}

void OnyxEditor::Maths::Transform::ScaleLocal(const glm::vec3& p_scale)
{
	SetLocalScale(glm::vec3(m_localScale.x * p_scale.x, m_localScale.y * p_scale.y, m_localScale.z * p_scale.z));
}

const glm::vec3& OnyxEditor::Maths::Transform::GetLocalPosition() const
{
	return m_localPosition;
}

const glm::quat& OnyxEditor::Maths::Transform::GetLocalRotation() const
{
	return m_localRotation;
}

const glm::vec3& OnyxEditor::Maths::Transform::GetLocalScale() const
{
	return m_localScale;
}

const glm::vec3& OnyxEditor::Maths::Transform::GetWorldPosition() const
{
	return m_worldPosition;
}

const glm::quat& OnyxEditor::Maths::Transform::GetWorldRotation() const
{
	return m_worldRotation;
}

const glm::vec3& OnyxEditor::Maths::Transform::GetWorldScale() const
{
	return m_worldScale;
}

const glm::mat4& OnyxEditor::Maths::Transform::GetLocalMatrix() const
{
	return m_localMatrix;
}

const glm::mat4& OnyxEditor::Maths::Transform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

glm::vec3 OnyxEditor::Maths::Transform::GetWorldForward() const
{
	return m_worldRotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 OnyxEditor::Maths::Transform::GetWorldUp() const
{
	return m_worldRotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 OnyxEditor::Maths::Transform::GetWorldRight() const
{
	return m_worldRotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 OnyxEditor::Maths::Transform::GetLocalForward() const
{
	return m_localRotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 OnyxEditor::Maths::Transform::GetLocalUp() const
{
	return m_localRotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 OnyxEditor::Maths::Transform::GetLocalRight() const
{
	return m_localRotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

void OnyxEditor::Maths::Transform::PreDecomposeWorldMatrix()
{
	m_worldPosition = glm::vec3(m_worldMatrix[3]);

	glm::vec3 columns[3] =
	{
		glm::vec3(m_worldMatrix[0]),
		glm::vec3(m_worldMatrix[1]),
		glm::vec3(m_worldMatrix[2]),
	};

	m_worldScale.x = glm::length(columns[0]);
	m_worldScale.y = glm::length(columns[1]);
	m_worldScale.z = glm::length(columns[2]);

	if (m_worldScale.x) columns[0] /= m_worldScale.x;
	if (m_worldScale.y) columns[1] /= m_worldScale.y;
	if (m_worldScale.z) columns[2] /= m_worldScale.z;

	glm::mat3 rotationMatrix(columns[0], columns[1], columns[2]);
	m_worldRotation = glm::quat_cast(rotationMatrix);
}

void OnyxEditor::Maths::Transform::PreDecomposeLocalMatrix()
{
	m_localPosition = glm::vec3(m_localMatrix[3]);

	glm::vec3 columns[3] =
	{
		glm::vec3(m_localMatrix[0]),
		glm::vec3(m_localMatrix[1]),
		glm::vec3(m_localMatrix[2]),
	};

	m_localScale.x = glm::length(columns[0]);
	m_localScale.y = glm::length(columns[1]);
	m_localScale.z = glm::length(columns[2]);

	if (m_localScale.x) columns[0] /= m_localScale.x;
	if (m_localScale.y) columns[1] /= m_localScale.y;
	if (m_localScale.z) columns[2] /= m_localScale.z;

	glm::mat3 rotationMatrix(columns[0], columns[1], columns[2]);
	m_localRotation = glm::quat_cast(rotationMatrix);
}

