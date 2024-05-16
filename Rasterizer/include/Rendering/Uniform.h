#pragma once

#include <string>

namespace Rendering
{
	template<typename T>
	struct Uniform
	{
		std::string ID;
		T Value;
	};
}
