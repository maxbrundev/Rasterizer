#include "AmberEditor/Resources/Parsers/OBJParser.h"

#include <algorithm>
#include <array>
#include <execution>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <sstream>

#include <glm/glm.hpp>

#include "AmberEditor/Resources/Parsers/MTLParser.h"
#include "AmberEditor/Tools/Utils/String.h"

struct VertexKeyHash
{
	std::size_t operator()(const AmberGL::Geometry::Vertex& p_vertexKey) const noexcept
	{
		auto hashCombine = [](std::size_t p_seed, float p_value) -> std::size_t
		{
			std::size_t hash = std::hash<float>{}(p_value);
			return p_seed ^ (hash + 0x9e3779b9 + (p_seed << 6) + (p_seed >> 2));
		};

		std::size_t seed = 0;
		seed = hashCombine(seed, p_vertexKey.position.x);
		seed = hashCombine(seed, p_vertexKey.position.y);
		seed = hashCombine(seed, p_vertexKey.position.z);
		seed = hashCombine(seed, p_vertexKey.texCoords.x);
		seed = hashCombine(seed, p_vertexKey.texCoords.y);
		seed = hashCombine(seed, p_vertexKey.normal.x);
		seed = hashCombine(seed, p_vertexKey.normal.y);
		seed = hashCombine(seed, p_vertexKey.normal.z);

		return seed;
	}
};

bool AmberEditor::Resources::Parsers::OBJParser::LoadOBJ(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData)
{
	m_directory = Tools::Utils::String::ExtractDirectoryFromPath(p_filePath);

	return ParseFile(p_filePath, p_meshes, p_materialsData);
}

bool AmberEditor::Resources::Parsers::OBJParser::ParseFile(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData)
{
	std::ifstream file(p_filePath);

	if (!file)
	{
		//TODO: Log.
		return false;
	}

	std::string fileContent;

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	fileContent = buffer.str();

	std::stringstream parser(fileContent);
	std::string line;

	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUvs;
	std::vector<glm::vec3> tempNormals;

	struct Group
	{
		std::string MaterialName;
		std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> Indices;
	};

	std::vector<Group> groups;
	Group currentGroup;
	currentGroup.MaterialName = "default";

	while (std::getline(parser, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		if (line.substr(0, 7) == "mtllib ")
		{
			p_materialsData = MTLParser::ParseMTL(m_directory + line.substr(7));
			
			for (auto& materialData : p_materialsData)
			{
				if (!materialData.DiffuseTexturePath.empty())
				{
					materialData.DiffuseTexturePath = m_directory + materialData.DiffuseTexturePath;
				}
			}
		}
		else if (line.find("v ") == 0)
		{
			ParseLine<glm::vec3>(line, "v %f%f%f", tempVertices);
		}
		else if (line.find("vt ") == 0)
		{
			ParseLine<glm::vec2>(line, "vt %f%f", tempUvs);
		}
		else if (line.find("vn ") == 0)
		{
			ParseLine<glm::vec3>(line, "vn %f%f%f", tempNormals);
		}
		else if (line.find("usemtl ") == 0)
		{
			if (!currentGroup.Indices.empty())
			{
				groups.push_back(currentGroup);
				currentGroup.Indices.clear();
			}
			currentGroup.MaterialName = line.substr(7);
		}
		else if (line.find("f ") == 0)
		{
			ParseIndices(line, currentGroup.Indices);
		}
	}

	if (!currentGroup.Indices.empty())
		groups.push_back(currentGroup);

	if (p_materialsData.empty())
	{
		p_materialsData.emplace_back("default", "");
	}

	struct MeshData
	{
		std::vector<AmberGL::Geometry::Vertex> Vertices;
		std::vector<uint32_t> Indices;
		uint8_t MaterialIndex = 0;
	};

	std::vector<MeshData> results(groups.size());
	std::vector<size_t> groupIndices(groups.size());
	std::iota(groupIndices.begin(), groupIndices.end(), 0);

	const int numVertices = static_cast<int>(tempVertices.size());
	const int numUvs      = static_cast<int>(tempUvs.size());
	const int numNormals  = static_cast<int>(tempNormals.size());

	std::for_each(std::execution::par, groupIndices.begin(), groupIndices.end(), [&](size_t p_index) 
	{
		const Group& group = groups[p_index];

		MeshData data;
		data.Vertices.reserve(group.Indices.size());
		data.Indices.reserve(group.Indices.size());

		std::unordered_map<AmberGL::Geometry::Vertex, uint32_t, VertexKeyHash> vertexMap;
		vertexMap.reserve(group.Indices.size());
		
		for (const auto& index : group.Indices)
		{
			int posIndex = std::get<0>(index);
			int uvIndex = std::get<1>(index);
			int normIndex = std::get<2>(index);

			if (posIndex < 0)
				posIndex = numVertices + (posIndex + 1);
			if (uvIndex < 0)
				uvIndex = numUvs + (uvIndex + 1);
			if (normIndex < 0)
				normIndex = numNormals + (normIndex + 1);

			AmberGL::Geometry::Vertex vertex
			{
				tempVertices[posIndex],
				(uvIndex >= 0 && uvIndex < numUvs) ? tempUvs[uvIndex] : glm::vec2{},
				(normIndex >= 0 && normIndex < numNormals) ? tempNormals[normIndex] : glm::vec3{}
			};

			auto it = vertexMap.find(vertex);
			if (it == vertexMap.end())
			{
				uint32_t newIndex = static_cast<uint32_t>(vertexMap.size());
				vertexMap[vertex] = newIndex;
				data.Vertices.push_back(vertex);
				data.Indices.push_back(newIndex);
			}
			else
			{
				data.Indices.push_back(it->second);
			}
		}

		auto it = std::find_if(p_materialsData.begin(), p_materialsData.end(), [&](const MaterialData& p_materialData)
		{
			return p_materialData.Name == group.MaterialName;
		});

		if (it != p_materialsData.end())
		{
			data.MaterialIndex = static_cast<uint8_t>(std::distance(p_materialsData.begin(), it));
		}
		else
		{
			data.MaterialIndex = 0;
		}

		results[p_index] = std::move(data);
	});

	for (const auto& data : results)
	{
		p_meshes.push_back(new Mesh(data.Vertices, data.Indices, data.MaterialIndex));
	}
	

	return true;
}

void AmberEditor::Resources::Parsers::OBJParser::ParseIndices(const std::string_view& p_line, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& p_indices)
{
	std::array<int, 12> indices = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	int indicesCount = sscanf_s(p_line.data(),
		"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
		&indices[0], &indices[1], &indices[2],
		&indices[3], &indices[4], &indices[5],
		&indices[6], &indices[7], &indices[8],
		&indices[9], &indices[10], &indices[11]);

	if (indicesCount == 12)
	{
		std::for_each(indices.begin(), indices.end(), [](int& p_indexValue) 
		{
			p_indexValue--;
		});

		p_indices.emplace_back(indices[0], indices[1], indices[2]);
		p_indices.emplace_back(indices[3], indices[4], indices[5]);
		p_indices.emplace_back(indices[6], indices[7], indices[8]);

		p_indices.emplace_back(indices[0], indices[1], indices[2]);
		p_indices.emplace_back(indices[6], indices[7], indices[8]);
		p_indices.emplace_back(indices[9], indices[10], indices[11]);

		return;
	}

	indicesCount = sscanf_s(p_line.data(), "f %d/%d/%d %d/%d/%d %d/%d/%d", 
		&indices[0], &indices[1], &indices[2],
		&indices[3], &indices[4], &indices[5],
		&indices[6], &indices[7], &indices[8]);

	if (indicesCount < 9)
	{
		indicesCount = sscanf_s(p_line.data(), "f %d/%d %d/%d %d/%d", 
			&indices[0], &indices[1],
			&indices[3], &indices[4],
			&indices[6], &indices[7]);

		if (indicesCount < 6)
		{
			sscanf_s(p_line.data(), "f %d//%d %d//%d %d//%d", 
				&indices[0], &indices[2],
				&indices[3], &indices[5],
				&indices[6], &indices[8]);
		}
	}

	std::for_each(indices.begin(), indices.end(), [](int& p_indexValue)
	{
		p_indexValue--;
	});

	p_indices.emplace_back(indices[0], indices[1], indices[2]);
	p_indices.emplace_back(indices[3], indices[4], indices[5]);
	p_indices.emplace_back(indices[6], indices[7], indices[8]);
}
