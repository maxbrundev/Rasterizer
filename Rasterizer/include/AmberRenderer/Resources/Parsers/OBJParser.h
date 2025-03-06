#pragma once

#include <string>
#include <vector>


#include "AmberRenderer/Resources/Parsers/MaterialData.h"
#include "AmberRenderer/Resources/Mesh.h"

namespace AmberRenderer::Resources::Parsers
{
	class OBJParser
	{
	public:
		bool LoadOBJ(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData);

	private:
		bool ParseFile(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData);

		template<typename T>
		void ParseLine(const std::string_view& p_line, const std::string_view& p_header, std::vector<T>& p_values);

		void ParseIndices(const std::string_view& p_line, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& p_indices);

	private:
		std::string m_directory;
	};

	template <typename T>
	void OBJParser::ParseLine(const std::string_view& p_line, const std::string_view& p_header, std::vector<T>& p_values)
	{
		T result;

		std::string scanStrFormat = std::string(p_header);

		constexpr size_t valueAmount = sizeof(T) / sizeof(float);

		for (size_t i = 0; i < valueAmount; ++i)
		{
			scanStrFormat += "%f";
		}

		if constexpr (valueAmount == 2)
		{
			sscanf_s(p_line.data(), scanStrFormat.c_str(), &result.x, &result.y);
		}
		else
		{
			sscanf_s(p_line.data(), scanStrFormat.c_str(), &result.x, &result.y, &result.z);
		}

		p_values.emplace_back(std::move(result));
	}
}
