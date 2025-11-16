#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	enum class ERenderingCapability : uint8_t
	{
		CULL_FACE,
		DEPTH_WRITE,
		DEPTH_TEST,
		MULTISAMPLE,
		STENCIL_TEST
	};
}
