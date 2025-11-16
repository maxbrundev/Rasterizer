#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Settings
{
	struct PassOrder
	{
		static constexpr uint32_t First = 0;
		static constexpr uint32_t Shadows = 100;
		static constexpr uint32_t Opaque = 200;
		static constexpr uint32_t Transparent = 300;
		static constexpr uint32_t Debug = 400;
	};
}
