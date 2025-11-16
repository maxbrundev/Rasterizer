#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	enum class EClearFlag : uint8_t
	{
		COLOR,
		DEPTH,
		STENCIL
	};
}