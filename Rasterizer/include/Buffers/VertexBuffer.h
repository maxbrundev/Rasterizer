#pragma once

#include <vector>

#include "Geometry/Vertex.h"

namespace Buffers
{
	struct VertexBuffer
	{
		std::vector<Geometry::Vertex> Vertices;

		VertexBuffer() = default;

		VertexBuffer(const std::vector<Geometry::Vertex>& p_vertices) : Vertices(p_vertices)
		{
		}
	};
}