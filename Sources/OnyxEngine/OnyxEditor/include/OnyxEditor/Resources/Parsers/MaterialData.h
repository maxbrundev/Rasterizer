#pragma once

#include <string>

#include <glm/glm.hpp>

namespace OnyxEditor::Resources::Parsers
{
	struct MaterialData
	{
		std::string Name;
		std::string DiffuseTexturePath;
		std::string MetallicRoughnessTexturePath;
		std::string NormalTexturePath;
		std::string EmissiveTexturePath;
		std::string AOTexturePath;
		std::string DisplacementTexturePath;

		glm::vec3 AmbientColor = glm::vec3(1.0f);
		glm::vec3 DiffuseColor = glm::vec3(1.0f);
		glm::vec3 SpecularColor = glm::vec3(0.5f);
		glm::vec3 EmissiveColor = glm::vec3(0.5f);

		float Opacity = 1.0f;
		float Shininess = 32.0f;
	};
}
