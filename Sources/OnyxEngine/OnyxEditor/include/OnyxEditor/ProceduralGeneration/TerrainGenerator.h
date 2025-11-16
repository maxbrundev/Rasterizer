#pragma once

#include <glm/glm.hpp>

#include "OnyxEditor/ProceduralGeneration/FastNoiseLite.h"

#include "OnyxEditor/Resources/Mesh.h"

namespace OnyxEditor::ProceduralGeneration
{
	class TerrainGenerator
	{
	public:
		static Resources::Mesh* GenerateTerrain(int p_resolution, float p_worldSize, const glm::vec2& p_gridCenter);

	private:
		static void Initialize();
		static float GetHeight(float p_x, float p_z);

	private:
		static FastNoiseLite FASTNOISE;
		static bool IS_INITIALIZED;
	};
}