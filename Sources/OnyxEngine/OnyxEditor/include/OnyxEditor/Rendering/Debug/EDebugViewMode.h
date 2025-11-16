#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Debug
{
	enum class EDebugViewMode : std::uint8_t
	{
		STANDARD,
		WIREFRAME,
		NORMALS,
		UV_COORDINATES,
		DEPTH_BUFFER,
		CHECKERBOARD_UV
	};
}
