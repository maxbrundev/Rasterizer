#pragma once

#include <string>
#include <vector>

#include "AmberRenderer/Resources/Parsers/MaterialData.h"

namespace AmberRenderer::Resources::Parsers
{
	class MTLParser
	{
	public:
		static std::vector<MaterialData> ParseMTL(const std::string& p_filePath);
	};
}
