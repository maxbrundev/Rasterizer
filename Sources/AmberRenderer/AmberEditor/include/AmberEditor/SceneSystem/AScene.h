#pragma once
#include <vector>

#include "AmberEditor/Resources/Model.h"

namespace AmberEditor::SceneSystem
{
	class AScene
	{
	public:
		AScene(const std::string& p_name);
		virtual ~AScene();

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float p_deltaTime) = 0;

		//Tempo in waiting for Entities wrapper
		std::vector<Resources::Model*>& GetModels();

	public:
		const std::string Name;
	protected:
		std::vector<Resources::Model*> m_models;
	};
}
