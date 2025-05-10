#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class EFramebufferAttachment : uint8_t
	{
		COLOR,
		DEPTH,
	};
}
