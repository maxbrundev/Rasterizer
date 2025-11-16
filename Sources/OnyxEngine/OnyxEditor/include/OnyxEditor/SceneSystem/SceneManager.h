#pragma once
#include "AScene.h"

namespace OnyxEditor::SceneSystem
{
	class SceneManager
	{
	public:
		SceneManager() = default;
		~SceneManager();

		void LoadScene(AScene* p_scene);
		void UnloadCurrentScene();

		bool HasCurrentScene() const;
		AScene* GetCurrentScene() const;

	private:
		AScene* m_currentScene = nullptr;
	};
}
