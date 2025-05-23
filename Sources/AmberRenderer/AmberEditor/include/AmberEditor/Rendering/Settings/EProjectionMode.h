#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class EProjectionMode : uint8_t
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
}