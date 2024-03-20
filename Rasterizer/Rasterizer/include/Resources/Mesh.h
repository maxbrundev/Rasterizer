#pragma once

#include <vector>

#include "Geometry/Vertex.h"

namespace Resources
{
	class Mesh final
	{
	public:
		Mesh() = default;

		void AddVertex(const Geometry::Vertex& p_vertex);
		void AddIndice(uint32_t p_indice);

		const std::vector<Geometry::Vertex>& GetVertices() const;

		const std::vector<uint32_t>& GetIndices() const;

	private:
		std::vector<Geometry::Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
	};
}
