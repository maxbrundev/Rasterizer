#pragma once

#include <string>
#include <any>

#include "OnyxEditor/Rendering/Settings/EUniformType.h"

namespace OnyxEditor::Resources
{
	struct UniformInfo
	{
		Rendering::Settings::EUniformType Type;
		std::string Name;
		uint16_t Location;
		std::any DefaultValue;
	};
}
