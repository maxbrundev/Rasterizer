#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include <glm/glm.hpp>

#include "Polygon.h"

namespace AmberGL::Geometry
{
	struct Plane
	{
		glm::vec3 Normal;
		float Distance;

		void ClipTriangle(Polygon& p_polygon) const;
		void ClipLine(Polygon& p_polygon) const;
		void ClipPoint(Polygon& p_polygon) const;

		float SignedDistance(const glm::vec4& p_point) const;
		bool IsInFront(const glm::vec4& p_point) const;
	};
}
