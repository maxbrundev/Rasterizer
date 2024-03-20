#include "Resources/Parsers/OBJParser.h"

#include <fstream>
#include <sstream>
#include <array>

#include <glm/glm.hpp>

void Resources::Parsers::OBJParser::LoadOBJ(const std::string& p_filePath, Mesh* p_mesh)
{
	ParseFile(p_filePath, p_mesh);
}

void Resources::Parsers::OBJParser::ParseFile(const std::string& p_filePath, Mesh* p_mesh)
{
	std::ifstream file(p_filePath);

	std::string fileContent;

	if (file)
	{
		std::stringstream buffer;

		buffer << file.rdbuf();

		file.close();

		fileContent = buffer.str();
	}
	else
	{
		fileContent = "";
	}

	std::stringstream parser{fileContent};
	std::string line;

	std::vector <std::tuple<uint32_t, uint32_t, uint32_t>> Indices;

	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUvs;
	std::vector<glm::vec3> tempNormals;

	while (std::getline(parser, line))
	{
		if (line.find("v ") == 0)
		{
			ParseLine<glm::vec3>(line, "v ", tempVertices);
		}
		else if (line.find("vt ") == 0)
		{
			ParseLine<glm::vec2>(line, "vt ", tempUvs);
		}
		else if (line.find("vn ") == 0)
		{
			ParseLine<glm::vec3>(line, "vn ",tempNormals);
		}
		else if (line.find("f ") == 0)
		{
			ParseIndices(line, Indices);
		}
	}

	std::vector<std::tuple<glm::vec3, glm::vec2, glm::vec3>> vertexData;

	for (size_t i = 0; i < Indices.size(); i++)
	{
		glm::vec3 position   = tempVertices[std::get<0>(Indices[i])];
		glm::vec2 textCoords = tempUvs.empty() ? glm::vec2{} : tempUvs[std::get<1>(Indices[i])];
		glm::vec3 normal     = tempNormals.empty() ? glm::vec3{} : tempNormals[std::get<2>(Indices[i])];

		std::tuple<glm::vec3, glm::vec2, glm::vec3> vertex = std::make_tuple(position, textCoords, normal);

		if (auto it = std::find(vertexData.begin(), vertexData.end(), vertex); it == vertexData.end())
		{
			vertexData.push_back(vertex);

			p_mesh->AddIndice(vertexData.size() - 1);
		}
		else
		{
			p_mesh->AddIndice(std::distance(vertexData.begin(), it));
		}
	}

	for (const auto&[position, textCoords, normal] : vertexData)
	{
		Geometry::Vertex vertex;
		vertex.position   = position;
		vertex.textCoords = textCoords;
		vertex.normal     = normal;

		p_mesh->AddVertex(vertex);
	}
}

void Resources::Parsers::OBJParser::ParseIndices(const std::string_view& p_line, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& p_indices)
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
