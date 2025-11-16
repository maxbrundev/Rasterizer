#pragma once

#include <string>

namespace OnyxEditor::Tools::Utils
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
		static std::string TrimRight(std::string p_source, const char p_character);
		static std::string TrimLeft(std::string p_source, const char p_character);
		static std::string Trim(std::string p_source, const char p_character);
	};
}
