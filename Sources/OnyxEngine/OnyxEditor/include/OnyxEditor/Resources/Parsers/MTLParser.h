#pragma once

#include <string>
#include <vector>

#include "OnyxEditor/Resources/Parsers/MaterialData.h"

namespace OnyxEditor::Resources::Parsers
{
	class MTLParser
	{
	public:
		static std::vector<MaterialData> ParseMTL(const std::string& p_filePath);
	};
}
