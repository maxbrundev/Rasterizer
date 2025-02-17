#include "Resources/Parsers/MTLParser.h"

#include "fstream"
#include <sstream>
#include <string>

//TODO: Handle edge cases.
std::vector<Resources::Parsers::MaterialData> Resources::Parsers::MTLParser::ParseMTL(const std::string& p_filePath)
{
	std::vector<MaterialData> materials;

	std::ifstream file(p_filePath);

	if (!file)
	{
		//TODO: Log.
		return materials;
	}

	MaterialData currentMaterial;

	std::string line;

	while (std::getline(file, line))
	{
		//TODO: Trim line.
		if (line.empty())
			continue;

		if (line.rfind("newmtl", 0) == 0)
		{
			if (!currentMaterial.Name.empty())
				materials.push_back(currentMaterial);

			std::istringstream isStringStream(line);
			std::string token;

			if (!(isStringStream >> token))
				continue;

			if (!(isStringStream >> currentMaterial.Name))
				continue;

			currentMaterial.DiffuseTexturePath.clear();
		}
		else if (line.rfind("map_Kd", 0) == 0)
		{
			std::istringstream isStringStream(line);
			std::string token;

			if (!(isStringStream >> token))
				continue;

			isStringStream >> currentMaterial.DiffuseTexturePath;
		}
	}

	if (!currentMaterial.Name.empty())
		materials.push_back(currentMaterial);

	return materials;
}
