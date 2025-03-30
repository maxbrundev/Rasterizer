#pragma once

#include <stdint.h>

namespace AmberRenderer::Rendering::SoftwareRenderer::Programs
{
	enum class EShaderDataType : uint8_t
	{
		INT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT2,
		MAT3,
		MAT4
	};
}
