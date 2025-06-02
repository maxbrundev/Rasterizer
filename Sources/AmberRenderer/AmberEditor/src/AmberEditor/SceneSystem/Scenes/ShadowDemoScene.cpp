#include "AmberEditor/SceneSystem/Scenes/ShadowDemoScene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "AmberEditor/Managers/ShaderManager.h"

#include "AmberEditor/Rendering/Renderer.h"
#include "AmberEditor/Rendering/Features/ShadowMappingFeature.h"
#include "AmberEditor/Resources/Loaders/ModelLoader.h"
#include "AmberEditor/Resources/Shaders/ShadowMapping.h"

#include "AmberEditor/Tools/Globals/ServiceLocator.h"

AmberEditor::SceneSystem::Scenes::ShadowDemoScene::ShadowDemoScene() : AScene("ShadowDemo")
{
}

AmberEditor::SceneSystem::Scenes::ShadowDemoScene::~ShadowDemoScene()
{
	delete m_cubeMaterial;
	delete m_planeMaterial;
}

void AmberEditor::SceneSystem::Scenes::ShadowDemoScene::Load()
{
	m_cubeModel = Resources::Loaders::ModelLoader::Create("Resources/Editor/Models/Cube.obj");
	
	planeModel = Resources::Loaders::ModelLoader::Create("Resources/Editor/Models/Cube.obj");
	m_models.push_back(planeModel);
	m_models.push_back(m_cubeModel);
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();
	auto* shadowMappingShader = shaderManager.GetShader<Resources::Shaders::ShadowMapping>("ShadowMap");

	if (shadowMappingShader)
	{
		m_cubeMaterial = new Resources::Material();
		m_cubeMaterial->SetShader(shadowMappingShader);
		m_cubeMaterial->SetName("CubeMaterial");

		m_planeMaterial = new Resources::Material();
		m_planeMaterial->SetShader(shadowMappingShader);
		m_planeMaterial->SetName("PlaneMaterial");

		m_cubeModel->FillWithMaterial(*m_cubeMaterial);
		planeModel->FillWithMaterial(*m_planeMaterial);

		// Generate model materials with textures, tempo workaround
		Resources::Loaders::ModelLoader::GenerateModelMaterials(*m_cubeModel);
		Resources::Loaders::ModelLoader::GenerateModelMaterials(*planeModel);
	}
}

void AmberEditor::SceneSystem::Scenes::ShadowDemoScene::Unload()
{
	if (m_cubeModel)
	{
		Resources::Loaders::ModelLoader::Destroy(m_cubeModel);
		m_cubeModel = nullptr;
	}

	if (planeModel)
	{
		Resources::Loaders::ModelLoader::Destroy(planeModel);
		planeModel = nullptr;
	}

	m_models.clear();

	delete m_cubeMaterial;
	m_cubeMaterial = nullptr;

	delete m_planeMaterial;
	m_planeMaterial = nullptr;
}

void AmberEditor::SceneSystem::Scenes::ShadowDemoScene::Update(float p_deltaTime)
{
	m_accumulatedTime += p_deltaTime;

	m_cubeRotationAngle += p_deltaTime * 65.0f;
	
	while (m_cubeRotationAngle > 360.0f)
	{
		m_cubeRotationAngle -= 360.0f;
	}

	if (m_cubeModel && m_cubeMaterial)
	{
		glm::mat4 cube1Model(1.0f);
		cube1Model = glm::translate(cube1Model, m_cube1Position);
		cube1Model = glm::rotate(cube1Model, glm::radians(m_cubeRotationAngle), glm::vec3(0.5f, 1.0f, 0.0f));

		m_cubeMaterial->SetUniform("u_Model", cube1Model);
	}

	if (m_planeMaterial)
	{
		glm::mat4 planeModel(1.0f);

		planeModel = glm::translate(planeModel, glm::vec3(0.0f, 1.0f, 2.0f));
		planeModel = glm::scale(planeModel, glm::vec3(5.0f, 0.2f, 5.0f));
		m_planeMaterial->SetUniform("u_Model", planeModel);
	}

	auto& renderer = Tools::Globals::ServiceLocator::Get<Rendering::Renderer>();

	auto& shadowMappingFeature = renderer.GetFeature<Rendering::Features::ShadowMappingFeature>();

	glm::vec3 lightPos;
	lightPos.x = m_lightOrbitRadius * cos(m_accumulatedTime * m_lightOrbitSpeed);
	lightPos.y = 6.0f;
	lightPos.z = m_lightOrbitRadius * sin(m_accumulatedTime * m_lightOrbitSpeed);

	shadowMappingFeature.SetLightPosition(lightPos);
}