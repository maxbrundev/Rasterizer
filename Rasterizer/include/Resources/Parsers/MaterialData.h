#pragma once

#include <string>

namespace Resources::Parsers
{
	struct MaterialData
	{
		std::string Name;
		std::string DiffuseTexturePath;

		MaterialData(const std::string& p_name = "", const std::string& p_diffuseTexturePath = "") : Name(p_name), DiffuseTexturePath(p_diffuseTexturePath)
		{
		}
	};
}
