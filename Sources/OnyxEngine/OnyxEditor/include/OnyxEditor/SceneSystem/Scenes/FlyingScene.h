#pragma once
#include <vector>
#include <map>

#include "OnyxEditor/SceneSystem/AScene.h"


namespace OnyxEditor::Controllers
{
	class PlaneController;
}

namespace OnyxEditor::SceneSystem::Scenes
{
	class FlyingScene : public AScene
	{
	public:
		FlyingScene();
		virtual ~FlyingScene() override;

		void Load() override;
		void Unload() override;
		void Update(float p_deltaTime) override;

	private:
		static constexpr int TERRAIN_RESOLUTION = 32;
		static constexpr float TERRAIN_CHUNK_SIZE = 100.0f;
		static constexpr int CHUNKS_GRID_SIZE = 5;
		static constexpr int HALF_CHUNKS_GRID_SIZE = CHUNKS_GRID_SIZE / 2;
		static constexpr int MAX_CHUNKS_PER_FRAME = 2;
		static constexpr int maxDistanceSquared = (HALF_CHUNKS_GRID_SIZE + 1) * (HALF_CHUNKS_GRID_SIZE + 1);

		using ChunkKey = std::pair<int, int>;
		std::map<ChunkKey, Resources::Model*> m_chunks;

		Controllers::PlaneController* m_airplaneController = nullptr;
		Resources::Model* m_propellerModel = nullptr;

		Resources::Material* m_terrainMaterial = nullptr;
		Resources::Material* m_standardMaterial = nullptr;
		Resources::Material* m_propellerMaterial = nullptr;

		int m_currentCenterX = 0;
		int m_currentCenterY = 0;
		float m_propellerRotation = 0.0f;

		void UpdateTerrainChunks();
		bool ChunkExists(int p_chunkX, int p_chunkY) const;
		void GenerateChunk(int p_chunkX, int p_chunkY);
		void UnloadDistantChunks();
		void UpdateCamera();
		void UpdatePropeller(float p_deltaTime);
	};
}
