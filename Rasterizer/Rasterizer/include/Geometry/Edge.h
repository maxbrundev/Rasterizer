#pragma once

#include "Vertex.h"

namespace Geometry
{
	struct Edge
	{
		Vertex a;
		Vertex b;

		float w;

		Edge(Vertex p_vertex0, Vertex p_vertex1) : a(p_vertex0), b(p_vertex1)
		{
			w = (p_vertex1.y - p_vertex0.y) - (p_vertex1.x - p_vertex0.x);
		}

		void Compute(Vertex p_point)
		{
			w *= (p_point.x - a.x) - (p_point.y - a.y);
		}
	};
}
