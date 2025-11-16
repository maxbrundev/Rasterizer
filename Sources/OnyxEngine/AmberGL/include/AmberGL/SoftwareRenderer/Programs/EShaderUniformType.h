#pragma once

#include <cstdint>

namespace AmberGL::SoftwareRenderer::Programs
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
		MAT4,
		SAMPLER_2D
	};
}
