#pragma once

namespace MathUtils
{
	float InterpolateBilinear(const float topLeft, const float topRight, const float bottomLeft, const float bottomRight, float xFactor, float yFactor);
}
