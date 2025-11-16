#include "OnyxEditor/Resources/Parsers/MTLParser.h"

#include <fstream>
#include <sstream>
#include <string>

#include "OnyxEditor/Tools/Time/Clock.h"
#include "OnyxEditor/Tools/Utils/String.h"

//TODO: Handle edge cases.
std::vector<OnyxEditor::Resources::Parsers::MaterialData> OnyxEditor::Resources::Parsers::MTLParser::ParseMTL(const std::string& p_filePath)
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
		if (line.empty())
			continue;

		std::string trimmed = Tools::Utils::String::Trim(line, ' ');
		trimmed = Tools::Utils::String::Trim(trimmed, '\t');
		std::string_view view(trimmed);

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
			currentMaterial.MetallicRoughnessTexturePath.clear();
			currentMaterial.NormalTexturePath.clear();
			currentMaterial.EmissiveTexturePath.clear();
			currentMaterial.AOTexturePath.clear();
			currentMaterial.DisplacementTexturePath.clear();
		}
		else if (view.substr(0, 7) == "map_Kd ")
		{
			currentMaterial.DiffuseTexturePath = line.substr(7);
			currentMaterial.DiffuseTexturePath = Tools::Utils::String::Trim(currentMaterial.DiffuseTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.DiffuseTexturePath, "\\", "/");
		}
		else if (view.substr(0, 7) == "map_Pr " || view.substr(0, 7) == "map_pr ")
		{
			currentMaterial.MetallicRoughnessTexturePath = line.substr(7);
			currentMaterial.MetallicRoughnessTexturePath = Tools::Utils::String::Trim(currentMaterial.MetallicRoughnessTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.MetallicRoughnessTexturePath, "\\", "/");
		}
		else if (view.substr(0, 8) == "map_Bump" || view.substr(0, 8) == "map_bump" || view.substr(0, 5) == "bump ")
		{
			size_t startPos = line.find_first_of(' ') + 1;
			currentMaterial.NormalTexturePath = line.substr(startPos);
			currentMaterial.NormalTexturePath = Tools::Utils::String::Trim(currentMaterial.NormalTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.NormalTexturePath, "\\", "/");
		}
		else if (view.substr(0, 7) == "map_Ke ")
		{
			currentMaterial.EmissiveTexturePath = line.substr(7);
			currentMaterial.EmissiveTexturePath = Tools::Utils::String::Trim(currentMaterial.EmissiveTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.EmissiveTexturePath, "\\", "/");
		}
		else if (view.substr(0, 10) == "map_Ka_ao " || view.substr(0, 7) == "map_ao ")
		{
			size_t startPos = line.find_first_of(' ') + 1;
			currentMaterial.AOTexturePath = line.substr(startPos);
			currentMaterial.AOTexturePath = Tools::Utils::String::Trim(currentMaterial.AOTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.AOTexturePath, "\\", "/");
		}
		else if (view.substr(0, 8) == "map_disp" || view.substr(0, 6) == "disp " || view.substr(0, 9) == "map_Disp ")
		{
			size_t startPos = line.find_first_of(' ') + 1;
			currentMaterial.DisplacementTexturePath = line.substr(startPos);
			currentMaterial.DisplacementTexturePath = Tools::Utils::String::Trim(currentMaterial.DisplacementTexturePath, ' ');
			Tools::Utils::String::ReplaceAll(currentMaterial.DisplacementTexturePath, "\\", "/");
		}
		else if (view.substr(0, 3) == "Ka ")
		{
			std::istringstream ss(std::string(view.substr(3)));
			ss >> currentMaterial.AmbientColor.r >> currentMaterial.AmbientColor.g >> currentMaterial.AmbientColor.b;
		}
		else if (view.substr(0, 3) == "Kd ")
		{
			std::istringstream ss(std::string(view.substr(3)));
			ss >> currentMaterial.DiffuseColor.r >> currentMaterial.DiffuseColor.g >> currentMaterial.DiffuseColor.b;
		}
		else if (view.substr(0, 3) == "Ks ")
		{
			std::istringstream ss(std::string(view.substr(3)));
			ss >> currentMaterial.SpecularColor.r >> currentMaterial.SpecularColor.g >> currentMaterial.SpecularColor.b;
		}
		else if (view.substr(0, 3) == "Ke ")
		{
			std::istringstream ss(std::string(view.substr(3)));
			ss >> currentMaterial.EmissiveColor.r >> currentMaterial.EmissiveColor.g >> currentMaterial.EmissiveColor.b;
		}
		else if (view.substr(0, 2) == "d ")
		{
			currentMaterial.Opacity = std::stof(std::string(view.substr(2)));
		}
		else if (view.substr(0, 3) == "Ns ")
		{
			currentMaterial.Shininess = std::stof(std::string(view.substr(3)));
		}
	}

	if (!currentMaterial.Name.empty())
	{
		materials.push_back(currentMaterial);
	}

	return materials;
}
