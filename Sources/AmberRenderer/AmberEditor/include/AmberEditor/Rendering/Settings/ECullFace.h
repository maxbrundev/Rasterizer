#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class ECullFace : uint8_t
	{
		BACK,
		FRONT,
		FRONT_AND_BACK
	};
}
