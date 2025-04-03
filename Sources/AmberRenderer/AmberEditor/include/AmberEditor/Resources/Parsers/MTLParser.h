#pragma once

#include <string>
#include <vector>

#include "AmberEditor/Resources/Parsers/MaterialData.h"

namespace AmberEditor::Resources::Parsers
{
	class MTLParser
	{
	public:
		static std::vector<MaterialData> ParseMTL(const std::string& p_filePath);
	};
}
