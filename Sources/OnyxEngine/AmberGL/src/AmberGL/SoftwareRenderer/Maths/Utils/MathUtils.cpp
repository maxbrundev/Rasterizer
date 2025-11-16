#include "AmberGL/Maths/Uttils/MathUtils.h"

float MathUtils::InterpolateBilinear(const float topLeft, const float topRight, const float bottomLeft, const float bottomRight, float xFactor, float yFactor)
{
	return (1.0f - xFactor) * (1.0f - yFactor) * topLeft
		+ xFactor * (1.0f - yFactor) * topRight
		+ (1.0f - xFactor) * yFactor * bottomLeft
		+ xFactor * yFactor * bottomRight;
}