#pragma once

#include <glm/glm.hpp>

#include "AmberEditor/SceneSystem/AScene.h"

#include "AmberEditor/Resources/Material.h"

namespace AmberEditor::SceneSystem::Scenes
{
	class ShadowDemoScene : public AScene
	{
	public:
		ShadowDemoScene();
		~ShadowDemoScene() override;

		void Load() override;
		void Unload() override;
		void Update(float p_deltaTime) override;

	private:
		Resources::Model* m_cubeModel = nullptr;
		Resources::Model* planeModel = nullptr;

		Resources::Material* m_cubeMaterial = nullptr;
		Resources::Material* m_planeMaterial = nullptr;

		uint32_t m_planeVAO = 0;
		uint32_t m_planeVBO = 0;
		uint32_t m_planeVertexCount = 0;

		float m_cubeRotationAngle = 0.0f;
		float m_accumulatedTime = 0.0f;

		float m_lightOrbitRadius = 5.0f;
		float m_lightOrbitSpeed = 0.5f;

		glm::vec3 m_cube1Position = glm::vec3(0.0f, 3.0f, 2.0f);
		glm::vec3 m_cube2Position = glm::vec3(0.0f, 1.0f, 2.0f);
		glm::vec3 m_cube2Scale = glm::vec3(2.0f, 0.2f, 2.0f);
	};
}
