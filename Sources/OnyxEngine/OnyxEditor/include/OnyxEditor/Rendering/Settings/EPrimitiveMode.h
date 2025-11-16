#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	enum class EPrimitiveMode : uint8_t
	{
		TRIANGLES,
		LINES,
		POINTS,
		TRIANGLE_STRIP
	};
}
