#include "AmberEditor/SceneSystem/AScene.h"

#include "AmberEditor/Managers/ModelManager.h"

AmberEditor::SceneSystem::AScene::AScene(const std::string& p_name) : Name(p_name)
{
}

AmberEditor::SceneSystem::AScene::~AScene()
{
	m_models.clear();
}

std::vector<AmberEditor::Resources::Model*>& AmberEditor::SceneSystem::AScene::GetModels()
{
	return m_models;
}
