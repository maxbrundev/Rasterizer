#pragma once

#include <string>
#include <vector>


#include "OnyxEditor/Resources/Parsers/MaterialData.h"
#include "OnyxEditor/Resources/Mesh.h"

namespace OnyxEditor::Resources::Parsers
{
	class OBJParser
	{
	public:
		bool LoadOBJ(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData);

	private:
		bool ParseFile(const std::string& p_filePath, std::vector<Mesh*>& p_meshes, std::vector<MaterialData>& p_materialsData);

		template<typename T>
		void ParseLine(const std::string_view& p_line, const std::string_view& p_format, std::vector<T>& p_values);

		void ParseIndices(const std::string_view& p_line, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& p_indices);

	private:
		std::string m_directory;

		std::vector<std::tuple<int, int, int>> m_indicesBuffer;
	};

	template <typename T>
	void OBJParser::ParseLine(const std::string_view& p_line, const std::string_view& p_format, std::vector<T>& p_values)
	{
		T result;

		if constexpr (sizeof(T) / sizeof(float) == 2)
		{
			sscanf_s(p_line.data(), p_format.data(), &result.x, &result.y);
		}
		else
		{
			sscanf_s(p_line.data(), p_format.data(), &result.x, &result.y, &result.z);
		}

		p_values.emplace_back(std::move(result));
	}
}
