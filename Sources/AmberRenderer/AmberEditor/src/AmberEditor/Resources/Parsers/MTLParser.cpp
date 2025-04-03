#include "AmberEditor/Resources/Parsers/MTLParser.h"

#include <fstream>
#include <sstream>
#include <string>

//TODO: Handle edge cases.
std::vector<AmberEditor::Resources::Parsers::MaterialData> AmberEditor::Resources::Parsers::MTLParser::ParseMTL(const std::string& p_filePath)
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
		std::string_view view(line);

		if (view.empty() || view.front() == '#')
			continue;

		if (view.substr(0, 7) == "newmtl ")
		{
			if (!currentMaterial.Name.empty())
				materials.push_back(currentMaterial);

			size_t pos = view.find_first_of(" \t", 7);

			if (pos == std::string::npos)
				pos = view.size();

			currentMaterial.Name = std::string(view.substr(7, pos - 7));
			currentMaterial.DiffuseTexturePath.clear();
		}
		else if (line.substr(0, 7) == "map_Kd ")
		{
			currentMaterial.DiffuseTexturePath = line.substr(7);
		}
	}

	if (!currentMaterial.Name.empty())
		materials.push_back(currentMaterial);

	return materials;
}
