#pragma once

#include <string>
#include <any>

#include "AmberEditor/Rendering/Settings/EUniformType.h"

namespace AmberEditor::Resources
{
	struct UniformInfo
	{
		Rendering::Settings::EUniformType Type;
		std::string Name;
		uint16_t Location;
		std::any DefaultValue;
	};
}
