#include "OnyxEditor/SceneSystem/Scenes/PlaneController.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "OnyxEditor/Inputs/InputManager.h"

#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::Controllers::PlaneController::PlaneController(Resources::Model* p_model) : m_model(p_model)
{
	UpdateTransform();
}

void OnyxEditor::Controllers::PlaneController::Update(float p_deltaTime)
{
	HandleInput(p_deltaTime);

	m_position += m_forward * p_deltaTime * SPEED;

	m_roll = glm::mix(m_roll, m_targetRoll, p_deltaTime * 6.0f);
	m_yaw = glm::mix(m_yaw, m_targetYaw, p_deltaTime * 5.0f);
	m_pitch = glm::mix(m_pitch, m_targetPitch, p_deltaTime * 3.0f);

	UpdateTransform();
}

void OnyxEditor::Controllers::PlaneController::HandleInput(float p_deltaTime)
{
	auto& inputManager = Tools::Globals::ServiceLocator::Get<Inputs::InputManager>();

	float deltaRoll = 0.0f;
	float deltaPitch = 0.0f;

	if (inputManager.GetKeyState(Inputs::EKey::KEY_A) == Inputs::EKeyState::KEY_DOWN)
	{
		deltaRoll += p_deltaTime * ROLL_SPEED;
	}

	if (inputManager.GetKeyState(Inputs::EKey::KEY_D) == Inputs::EKeyState::KEY_DOWN)
	{
		deltaRoll -= p_deltaTime * ROLL_SPEED;
	}

	if (inputManager.GetKeyState(Inputs::EKey::KEY_W) == Inputs::EKeyState::KEY_DOWN)
	{
		deltaPitch -= p_deltaTime * PITCH_SPEED;
	}

	if (inputManager.GetKeyState(Inputs::EKey::KEY_S) == Inputs::EKeyState::KEY_DOWN)
	{
		deltaPitch += p_deltaTime * PITCH_SPEED;
	}

	if (inputManager.GetKeyState(Inputs::EKey::KEY_LSHIFT) == Inputs::EKeyState::KEY_DOWN)
	{
		deltaRoll *= 0.4f;
		deltaPitch *= 0.4f;
	}

	if (deltaRoll == 0.0f) 
		m_targetRoll = 0.0f;

	if (deltaPitch == 0.0f) 
		m_targetPitch = 0.0f;

	m_targetRoll -= deltaRoll;
	m_targetPitch += deltaPitch;
	m_targetYaw += deltaRoll * 0.5f;

	m_targetPitch = std::clamp(m_targetPitch, -PITCH_MAX, PITCH_MAX);
	m_targetRoll = std::clamp(m_targetRoll, -ROLL_MAX, ROLL_MAX);
}

void OnyxEditor::Controllers::PlaneController::UpdateTransform()
{
	glm::quat pitchQuat = glm::angleAxis(m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat yawQuat = glm::angleAxis(m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat rollQuat = glm::angleAxis(m_roll, glm::vec3(0.0f, 0.0f, 1.0f));

	m_currentRotation = yawQuat * pitchQuat * rollQuat;

	m_forward = m_currentRotation * glm::vec3(0.0f, 0.0f, 1.0f);

	if (m_model)
	{
		m_model->Transform.SetLocalPosition(m_position);
		m_model->Transform.SetLocalRotation(m_currentRotation);
	}
}
