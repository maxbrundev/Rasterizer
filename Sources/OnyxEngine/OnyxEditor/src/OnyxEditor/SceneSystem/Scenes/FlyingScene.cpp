#include "OnyxEditor/SceneSystem/Scenes/FlyingScene.h"

#include "OnyxEditor/Managers/ModelManager.h"
#include "OnyxEditor/Managers/ShaderManager.h"

#include "OnyxEditor/ProceduralGeneration/TerrainGenerator.h"

#include "OnyxEditor/Resources/Loaders/ModelLoader.h"
#include "OnyxEditor/Resources/Shaders/StandardShader.h"
#include "OnyxEditor/Resources/Shaders/TerrainShader.h"

#include "OnyxEditor/SceneSystem/Scenes/PlaneController.h"

#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::SceneSystem::Scenes::FlyingScene::FlyingScene() : AScene("FlyingScene")
{
	m_cameraPosition = glm::vec3(0.0f, 0.0f, 20.0f);
	m_cameraRotation = glm::quat(glm::vec3(glm::radians(-15.0f), 0.0f, 0.0f));
}

OnyxEditor::SceneSystem::Scenes::FlyingScene::~FlyingScene()
{
	Unload();
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::Load()
{
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();

	auto* terrainShader = shaderManager.GetShader<Resources::Shaders::TerrainShader>("TerrainShader");
	m_terrainMaterial = new Resources::Material();
	m_terrainMaterial->SetShader(terrainShader);
	m_terrainMaterial->SetName("TerrainMaterial");

	auto* standardShader = shaderManager.GetShader<Resources::Shaders::StandardShader>("StandardShader");
	m_standardMaterial = new Resources::Material();
	m_standardMaterial->SetShader(standardShader);
	m_standardMaterial->SetName("StandardMaterial");
	m_standardMaterial->SetUniform("u_AlbedoColor", glm::vec3(1.0f, 0.0f, 0.0f));

	m_propellerMaterial = new Resources::Material();
	m_propellerMaterial->SetShader(standardShader);
	m_propellerMaterial->SetName("PropellerMaterial");
	m_propellerMaterial->SetUniform("u_AlbedoColor", glm::vec3(1.0f, 0.0f, 0.0f));

	auto& modelManager = Tools::Globals::ServiceLocator::Get<Managers::ModelManager>();

	Resources::Model* airplaneModel = modelManager.LoadResource(":Models/biplane.obj");
	if (airplaneModel)
	{
		airplaneModel->FillWithMaterial(*m_standardMaterial);
		m_airplaneController = new Controllers::PlaneController(airplaneModel);
		m_models.push_back(airplaneModel);
	}

	m_propellerModel = modelManager.LoadResource(":Models/propeller.obj");

	if (m_propellerModel)
	{
		m_propellerModel->FillWithMaterial(*m_propellerMaterial);
		m_propellerModel->Transform.SetParent(airplaneModel->Transform);
		m_propellerModel->Transform.SetLocalRotation(glm::quat(glm::vec3(0.0f)));
		m_models.push_back(m_propellerModel);
	}

	UpdateTerrainChunks();
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::Unload()
{
	for (auto& [key, model] : m_chunks)
	{
		auto it = std::find(m_models.begin(), m_models.end(), model);
		if (it != m_models.end())
		{
			m_models.erase(it);
		}

		delete model;
	}

	m_chunks.clear();

	if (m_propellerModel)
	{
		Resources::Loaders::ModelLoader::Destroy(m_propellerModel);
		m_propellerModel = nullptr;
	}

	delete m_terrainMaterial;
	m_terrainMaterial = nullptr;

	delete m_standardMaterial;
	m_standardMaterial = nullptr;

	delete m_propellerMaterial;
	m_propellerMaterial = nullptr;

	delete m_airplaneController;
	m_airplaneController = nullptr;
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::Update(float p_deltaTime)
{
	m_airplaneController->Update(p_deltaTime);
	UpdatePropeller(p_deltaTime);
	UpdateCamera();
	UpdateTerrainChunks();
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::UpdateTerrainChunks()
{
	const glm::vec3& airplanePosition = m_airplaneController->GetPosition();

	int centerX = static_cast<int>(std::round(airplanePosition.x / TERRAIN_CHUNK_SIZE));
	int centerY = static_cast<int>(std::round(airplanePosition.z / TERRAIN_CHUNK_SIZE));

	m_currentCenterX = centerX;
	m_currentCenterY = centerY;

	UnloadDistantChunks();

	int chunksGeneratedCount = 0;

	for (int y = centerY - HALF_CHUNKS_GRID_SIZE; y <= centerY + HALF_CHUNKS_GRID_SIZE; ++y)
	{
		for (int x = centerX - HALF_CHUNKS_GRID_SIZE; x <= centerX + HALF_CHUNKS_GRID_SIZE; ++x)
		{
			if (!ChunkExists(x, y) && chunksGeneratedCount < MAX_CHUNKS_PER_FRAME)
			{
				GenerateChunk(x, y);
				chunksGeneratedCount++;
			}
		}
	}
}

bool OnyxEditor::SceneSystem::Scenes::FlyingScene::ChunkExists(int p_chunkX, int p_chunkY) const
{
	return m_chunks.find({ p_chunkX, p_chunkY }) != m_chunks.end();
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::GenerateChunk(int p_chunkX, int p_chunkY)
{
	glm::vec2 chunkCenter = glm::vec2(p_chunkX * TERRAIN_CHUNK_SIZE, p_chunkY * TERRAIN_CHUNK_SIZE);

	Resources::Mesh* terrainMesh = ProceduralGeneration::TerrainGenerator::GenerateTerrain(TERRAIN_RESOLUTION, TERRAIN_CHUNK_SIZE, chunkCenter);

	Resources::Model* terrainModel = new Resources::Model("TerrainChunk_" + std::to_string(p_chunkX) + "_" + std::to_string(p_chunkY));
	terrainModel->GetMeshes().push_back(terrainMesh);
	terrainModel->FillWithMaterial(*m_terrainMaterial);

	m_chunks[{p_chunkX, p_chunkY}] = terrainModel;
	m_models.push_back(terrainModel);
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::UnloadDistantChunks()
{
	std::vector<ChunkKey> toRemove;

	for (const auto& [key, model] : m_chunks)
	{
		int deltaX = key.first - m_currentCenterX;
		int deltaY = key.second - m_currentCenterY;

		int distanceSquared = deltaX * deltaX + deltaY * deltaY;

		if (distanceSquared > maxDistanceSquared)
		{
			toRemove.push_back(key);
		}
	}

	for (const auto& key : toRemove)
	{
		auto it = m_chunks.find(key);
		if (it != m_chunks.end())
		{
			auto modelIt = std::find(m_models.begin(), m_models.end(), it->second);
			if (modelIt != m_models.end())
			{
				m_models.erase(modelIt);
			}
			delete it->second;
			m_chunks.erase(it);
		}
	}
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::UpdateCamera()
{
	glm::vec3 airplanePos = m_airplaneController->GetPosition();
	glm::vec3 forward = m_airplaneController->GetForward();

	glm::vec3 targetForward = glm::vec3(forward.x, 0.0f, forward.z);

	if (glm::length(targetForward) > 0.0f)
	{
		targetForward = glm::normalize(targetForward);
	}
	else
	{
		targetForward = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	m_cameraPosition = airplanePos - targetForward * 5.0f + glm::vec3(0.0f, 1.5f, 0.0f);

	float yaw = m_airplaneController->GetYaw();
	m_cameraRotation = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
}

void OnyxEditor::SceneSystem::Scenes::FlyingScene::UpdatePropeller(float p_deltaTime)
{
	m_propellerRotation += p_deltaTime * 12.0f;
	m_propellerModel->Transform.SetLocalRotation(glm::angleAxis(m_propellerRotation, glm::vec3(0.0f, 0.0f, 1.0f)));
}