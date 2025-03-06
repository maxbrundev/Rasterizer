#include "AmberRenderer/Resources/Parsers/OBJParser.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <map>
#include <sstream>

#include <glm/glm.hpp>

#include "AmberRenderer/Resources/Parsers/MTLParser.h"
#include "AmberRenderer/Tools/Utils/String.h"

bool AmberRenderer::Resources::Parsers::OBJParser::LoadOBJ(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData)
{
	m_directory = Tools::Utils::String::ExtractDirectoryFromPath(p_filePath);

	return ParseFile(p_filePath, p_meshes, p_materialsData);
}

bool AmberRenderer::Resources::Parsers::OBJParser::ParseFile(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData)
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
			ParseLine<glm::vec3>(line, "v ", tempVertices);
		}
		else if (line.find("vt ") == 0)
		{
			ParseLine<glm::vec2>(line, "vt ", tempUvs);
		}
		else if (line.find("vn ") == 0)
		{
			ParseLine<glm::vec3>(line, "vn ", tempNormals);
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

	for (const Group& group : groups)
	{
		std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> vertexData;
		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;

		for (const auto& index : group.Indices)
		{
			glm::vec3 position  = tempVertices[std::get<0>(index)];
			glm::vec2 texCoords = std::get<1>(index) < tempUvs.size() ? tempUvs[std::get<1>(index)] : glm::vec2{};
			glm::vec3 normal    = std::get<2>(index) < tempNormals.size() ? tempNormals[std::get<2>(index)] : glm::vec3{};

			std::tuple<glm::vec3, glm::vec2, glm::vec3> vertex = std::make_tuple(position, texCoords, normal);

			if (auto it = std::find(vertexData.begin(), vertexData.end(), vertex); it == vertexData.end())
			{
				vertexData.push_back(vertex);
				indices.push_back(static_cast<uint32_t>(vertexData.size() - 1));
			}
			else
			{
				indices.push_back(static_cast<uint32_t>(std::distance(vertexData.begin(), it)));
			}
		}
		for (const auto& [position, texCoords, normal] : vertexData)
		{
			Geometry::Vertex vert;
			vert.position = position;
			vert.texCoords = texCoords;
			vert.normal = normal;

			vertices.push_back(vert);
		}

		uint8_t materialIndex = 0;

		auto it = std::find_if(p_materialsData.begin(), p_materialsData.end(), [&](const MaterialData& p_materialData)
		{
			return p_materialData.Name == group.MaterialName;
		});

		if (it != p_materialsData.end())
		{
			materialIndex = static_cast<uint8_t>(std::distance(p_materialsData.begin(), it));
		}

		p_meshes.push_back(new Mesh(vertices, indices, materialIndex));
	}

	return true;
}

void AmberRenderer::Resources::Parsers::OBJParser::ParseIndices(const std::string_view& p_line, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& p_indices)
{
	std::array<int, 9> indices = { -1, -1, -1, -1, -1, -1, -1, -1, -1 };

	int count = sscanf_s(p_line.data(), "f %d/%d/%d %d/%d/%d %d/%d/%d", 
		&indices[0], &indices[1], &indices[2],
		&indices[3], &indices[4], &indices[5],
		&indices[6], &indices[7], &indices[8]);

	if (count < 9)
	{
		count = sscanf_s(p_line.data(), "f %d/%d %d/%d %d/%d", 
			&indices[0], &indices[1],
			&indices[3], &indices[4],
			&indices[6], &indices[7]);

		if (count < 6)
		{
			sscanf_s(p_line.data(), "f %d//%d %d//%d %d//%d", 
				&indices[0], &indices[2],
				&indices[3], &indices[5],
				&indices[6], &indices[8]);
		}
	}

	std::for_each(indices.begin(), indices.end(), [](int& indexValue)
	{
		indexValue--;
	});

	p_indices.emplace_back(indices[0], indices[1], indices[2]);
	p_indices.emplace_back(indices[3], indices[4], indices[5]);
	p_indices.emplace_back(indices[6], indices[7], indices[8]);
}
