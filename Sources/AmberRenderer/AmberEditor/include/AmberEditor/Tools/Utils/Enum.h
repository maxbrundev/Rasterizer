#pragma once

#include <array>
#include <utility>

#include "AmberEditor/Rendering/AmberGLBindings.h"

template <typename Value, typename Enum>
constexpr Value GetEnumValue(Enum p_enum)
{
	constexpr auto enumMap = EnumValueTypeTraits<Enum, Value>::EnumMap;

	for (auto enumValuePair : enumMap)
	{
		if (enumValuePair.first == p_enum)
		{
			return enumValuePair.second;
		}
	}
	return Value();
}
