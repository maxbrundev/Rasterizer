#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class EPrimitiveMode : uint8_t
	{
		TRIANGLES,
		LINES,
		POINTS,
		TRIANGLE_STRIP
	};
}
