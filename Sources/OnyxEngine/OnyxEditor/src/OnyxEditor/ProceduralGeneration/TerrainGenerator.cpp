#include "OnyxEditor/ProceduralGeneration/TerrainGenerator.h"

#include <algorithm>

FastNoiseLite OnyxEditor::ProceduralGeneration::TerrainGenerator::FASTNOISE;
bool OnyxEditor::ProceduralGeneration::TerrainGenerator::IS_INITIALIZED = false;

void OnyxEditor::ProceduralGeneration::TerrainGenerator::Initialize()
{
	if (IS_INITIALIZED) 
		return;

	FASTNOISE.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	FASTNOISE.SetSeed(42);
	FASTNOISE.SetFrequency(0.012f);
	FASTNOISE.SetFractalType(FastNoiseLite::FractalType_FBm);
	FASTNOISE.SetFractalOctaves(4);
	FASTNOISE.SetFractalLacunarity(2.0f);
	FASTNOISE.SetFractalGain(0.5f);

	IS_INITIALIZED = true;
}

float OnyxEditor::ProceduralGeneration::TerrainGenerator::GetHeight(float p_x, float p_z)
{
	float noise = FASTNOISE.GetNoise(p_x, p_z); // [-1, 1]

	float height = noise * 22.0f;

	height = std::max(height, -1.5f);

	if (height > 4.0f)
	{
		float excess = height - 4.0f;
		height = 4.0f + excess * 1.2f;
	}

	return height;
}

OnyxEditor::Resources::Mesh* OnyxEditor::ProceduralGeneration::TerrainGenerator::GenerateTerrain(int p_resolution, float p_worldSize, const glm::vec2& p_gridCenter)
{
	Initialize();

	std::vector<std::vector<glm::vec3>> heightMap(p_resolution, std::vector<glm::vec3>(p_resolution));

	float step = p_worldSize / (p_resolution - 1);
	float halfSize = p_worldSize * 0.5f;

	for (int z = 0; z < p_resolution; z++)
	{
		for (int x = 0; x < p_resolution; x++)
		{
			float worldX = p_gridCenter.x - halfSize + x * step;
			float worldZ = p_gridCenter.y - halfSize + z * step;
			float height = GetHeight(worldX, worldZ);

			heightMap[x][z] = glm::vec3(worldX, height, worldZ);
		}
	}

	std::vector<AmberGL::Geometry::Vertex> vertices;
	vertices.reserve((p_resolution - 1) * (p_resolution - 1) * 6);

	for (int z = 0; z < p_resolution - 1; z++)
	{
		for (int x = 0; x < p_resolution - 1; x++)
		{
			glm::vec3 v0 = heightMap[x][z];
			glm::vec3 v1 = heightMap[x][z + 1];
			glm::vec3 v2 = heightMap[x + 1][z];
			glm::vec3 v3 = heightMap[x + 1][z + 1];

			// Triangle 1
			glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
			float avgHeight1 = (v0.y + v1.y + v2.y) / 3.0f;
			glm::vec2 texCoord1 = glm::vec2(avgHeight1, 0.0f);

			vertices.push_back({ v0, texCoord1, normal1 });
			vertices.push_back({ v1, texCoord1, normal1 });
			vertices.push_back({ v2, texCoord1, normal1 });

			// Triangle 2
			glm::vec3 normal2 = glm::normalize(glm::cross(v1 - v2, v3 - v2));
			float avgHeight2 = (v2.y + v1.y + v3.y) / 3.0f;
			glm::vec2 texCoord2 = glm::vec2(avgHeight2, 0.0f);

			vertices.push_back({ v2, texCoord2, normal2 });
			vertices.push_back({ v1, texCoord2, normal2 });
			vertices.push_back({ v3, texCoord2, normal2 });
		}
	}

	std::vector<uint32_t> indices;
	indices.reserve(vertices.size());
	for (uint32_t i = 0; i < vertices.size(); i++)
	{
		indices.push_back(i);
	}

	return new Resources::Mesh(vertices, indices, 0);
}