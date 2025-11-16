#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	enum class EFramebufferAttachment : uint8_t
	{
		COLOR,
		DEPTH,
	};
}
