#include "AmberEditor/SceneSystem/SceneManager.h"

AmberEditor::SceneSystem::SceneManager::~SceneManager()
{
	UnloadCurrentScene();
}

void AmberEditor::SceneSystem::SceneManager::LoadScene(AScene* p_scene)
{
	if (m_currentScene)
	{
		UnloadCurrentScene();
	}

	m_currentScene = p_scene;
	
	m_currentScene->Load();
}

void AmberEditor::SceneSystem::SceneManager::UnloadCurrentScene()
{
	if (m_currentScene)
	{
		m_currentScene->Unload();
		delete m_currentScene;
		m_currentScene = nullptr;
	}
}

bool AmberEditor::SceneSystem::SceneManager::HasCurrentScene() const
{
	return m_currentScene != nullptr;
}

AmberEditor::SceneSystem::AScene* AmberEditor::SceneSystem::SceneManager::GetCurrentScene() const
{
	return m_currentScene;
}
