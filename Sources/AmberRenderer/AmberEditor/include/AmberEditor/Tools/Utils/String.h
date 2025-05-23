#pragma once

#include <string>

namespace AmberEditor::Tools::Utils
{
	class String
	{
	public:
		String() = delete;

		static std::string ExtractDirectoryFromPath(const std::string& p_path);
		static std::string ExtractFileNameFromPath(const std::string& p_path);
		static std::string RemoveExtensionFromFileName(const std::string& p_file);
		static bool Replace(std::string& p_target, const std::string& p_from, const std::string& p_to);
		static void ReplaceAll(std::string& p_target, const std::string& p_from, const std::string& p_to);
	};
}
