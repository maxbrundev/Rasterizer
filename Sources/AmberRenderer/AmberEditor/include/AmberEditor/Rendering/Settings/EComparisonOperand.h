#pragma once

#include <cstdint>

namespace AmberEditor::Rendering::Settings
{
	enum class EComparisonOperand : uint8_t
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};
}