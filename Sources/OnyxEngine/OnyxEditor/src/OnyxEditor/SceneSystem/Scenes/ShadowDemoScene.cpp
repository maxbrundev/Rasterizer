#include "OnyxEditor/SceneSystem/Scenes/ShadowDemoScene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "OnyxEditor/Managers/ModelManager.h"
#include "OnyxEditor/Managers/ShaderManager.h"

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"
#include "OnyxEditor/Rendering/Passes/SelectedPass.h"
#include "OnyxEditor/Resources/Loaders/ModelLoader.h"
#include "OnyxEditor/Resources/Shaders/ShadowMapping.h"

#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::SceneSystem::Scenes::ShadowDemoScene::ShadowDemoScene() : AScene("ShadowDemo")
{
	m_cameraPosition = glm::vec3(0.0f, 4.0f, 12.0f);
	glm::vec3 eulerAngles = glm::vec3(glm::radians(-5.0f), glm::radians(180.0f), 0.0f);
	m_cameraRotation = glm::quat(eulerAngles);
}

OnyxEditor::SceneSystem::Scenes::ShadowDemoScene::~ShadowDemoScene()
{
	delete m_cubeMaterial;
	delete m_planeMaterial;
}

void OnyxEditor::SceneSystem::Scenes::ShadowDemoScene::Load()
{
	auto& modelManager = Tools::Globals::ServiceLocator::Get<Managers::ModelManager>();

	m_cubeModel = modelManager.LoadResource(":Models/DamagedHelmet/wavefront/DamagedHelmet.obj"/*"Resources/Editor/Models/Sponza/sponza.obj"*/);
	
	planeModel = modelManager.LoadResource(":Models/Cube.obj");
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

		Resources::Loaders::ModelLoader::GenerateModelMaterials(*m_cubeModel);
		Resources::Loaders::ModelLoader::GenerateModelMaterials(*planeModel);

		m_cubeMaterial->SetUniform("u_DisplacementScale", 0.0f);
		m_cubeMaterial->SetUniform("u_EmissiveStrength", 1.0f);
		m_planeMaterial->SetUniform("u_DisplacementScale", 0.05f);
	}

	auto& renderer = Tools::Globals::ServiceLocator::Get<Rendering::Renderer>();
	auto& selectedPass = renderer.GetPass<Rendering::Passes::SelectedPass>("SelectedPass");
	selectedPass.SetSelectedModel(m_cubeModel);
}

void OnyxEditor::SceneSystem::Scenes::ShadowDemoScene::Unload()
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

void OnyxEditor::SceneSystem::Scenes::ShadowDemoScene::Update(float p_deltaTime)
{
	m_cameraController.Update(p_deltaTime);
	m_accumulatedTime += p_deltaTime;

	m_cubeRotationAngle += p_deltaTime * 65.0f;
	
	while (m_cubeRotationAngle > 360.0f)
	{
		m_cubeRotationAngle -= 360.0f;
	}

	if (m_cubeModel && m_cubeMaterial)
	{
		m_cubeModel->Transform.SetLocalPosition(m_cube1Position);
	}

	if (m_planeMaterial)
	{
		planeModel->Transform.SetLocalPosition(glm::vec3(0.0f, 1.0f, 2.0f));
		planeModel->Transform.SetLocalScale(glm::vec3(5.0f, 0.2f, 5.0f));
	}

	auto& renderer = Tools::Globals::ServiceLocator::Get<Rendering::Renderer>();
	
	auto& shadowMappingFeature = renderer.GetFeature<Rendering::Features::ShadowMappingRenderFeature>();
	
	glm::vec3 lightPos;
	lightPos.x = m_lightOrbitRadius * cos(m_accumulatedTime * m_lightOrbitSpeed);
	lightPos.y = 6.0f;
	lightPos.z = m_lightOrbitRadius * sin(m_accumulatedTime * m_lightOrbitSpeed);
	
	shadowMappingFeature.SetLightPosition(lightPos);
}