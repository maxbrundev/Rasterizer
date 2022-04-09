#pragma once

#include "Vertex.h"

namespace Geometry
{
	struct Edge
	{
		Vertex vertex0;
		Vertex vertex1;

		float a;
		float b;
		float c;
		bool isTie;

		Edge(Vertex p_vertex0, Vertex p_vertex1) :
		vertex0(p_vertex0),
		vertex1(p_vertex1),
		a(p_vertex1.y - p_vertex0.y),
		b(p_vertex0.x - p_vertex1.x),
		c(-(a * (p_vertex1.x + p_vertex0.x) + b * (p_vertex1.y + p_vertex0.y)) / 2)
		{
			if (a != 0.0f)
			{
				isTie = a > 0.0f;
			}
			else
			{
				isTie = b > 0.0f;
			}
		}

		float Evaluate(float p_positionX, float p_positionY) const
		{
			return a * p_positionX + b * p_positionY + c;
		}

		bool IsInsideEdge(float p_positionX, float p_positionY) const
		{
			const float result = Evaluate(p_positionX, p_positionY);

			return (result > 0.0f || result == 0.0f && isTie);
		}
	};
}
