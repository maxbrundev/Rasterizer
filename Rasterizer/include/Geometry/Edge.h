#pragma once

#include "Vertex.h"

namespace Geometry
{
	struct Edge
	{
		Vertex a;
		Vertex b;

		Vertex edge;

		float area = 0.0f;

		Edge(Vertex p_vertex0, Vertex p_vertex1) : a(p_vertex0), b(p_vertex1)
		{
			edge.x = b.x - a.x;
			edge.y = b.y - a.y;
		}

		void ComputeArea(Vertex p_point)
		{
			area = (p_point.x - a.x) * edge.y - (p_point.y - a.y) * edge.x;
		}
	};
}
