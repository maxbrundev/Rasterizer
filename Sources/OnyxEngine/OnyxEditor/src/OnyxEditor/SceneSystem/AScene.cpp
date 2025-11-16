#include "OnyxEditor/SceneSystem/AScene.h"

OnyxEditor::SceneSystem::AScene::AScene(const std::string& p_name) :
Name(p_name),
m_cameraController(m_camera, m_cameraPosition, m_cameraRotation),
m_cameraPosition(0.0f, 0.0f, 10.0f),
m_cameraRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)))
{
	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);
}

OnyxEditor::SceneSystem::AScene::~AScene()
{
}

std::vector<OnyxEditor::Resources::Model*>& OnyxEditor::SceneSystem::AScene::GetModels()
{
	return m_models;
}