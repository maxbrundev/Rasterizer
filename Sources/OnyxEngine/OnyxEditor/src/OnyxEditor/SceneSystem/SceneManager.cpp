#include "OnyxEditor/SceneSystem/SceneManager.h"

OnyxEditor::SceneSystem::SceneManager::~SceneManager()
{
	UnloadCurrentScene();
}

void OnyxEditor::SceneSystem::SceneManager::LoadScene(AScene* p_scene)
{
	if (m_currentScene)
	{
		UnloadCurrentScene();
	}

	m_currentScene = p_scene;
	
	m_currentScene->Load();
}

void OnyxEditor::SceneSystem::SceneManager::UnloadCurrentScene()
{
	if (m_currentScene)
	{
		m_currentScene->Unload();
		delete m_currentScene;
		m_currentScene = nullptr;
	}
}

bool OnyxEditor::SceneSystem::SceneManager::HasCurrentScene() const
{
	return m_currentScene != nullptr;
}

OnyxEditor::SceneSystem::AScene* OnyxEditor::SceneSystem::SceneManager::GetCurrentScene() const
{
	return m_currentScene;
}
