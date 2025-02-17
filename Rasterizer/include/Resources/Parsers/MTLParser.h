#pragma once

#include <string>
#include <vector>

#include "Resources/Parsers/MaterialData.h"

namespace Resources::Parsers
{
	class MTLParser
	{
	public:
		static std::vector<MaterialData> ParseMTL(const std::string& p_filePath);
	};
}
