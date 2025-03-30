#pragma once

#include <memory>
#include <vector>

#include "AmberRenderer/Resources/IMesh.h"

#include "AmberRenderer/Buffers/VertexBuffer.h"
#include "AmberRenderer/Buffers/IndexBuffer.h"
#include "AmberRenderer/Buffers/VertexArray.h"
#include "AmberRenderer/Geometry/Vertex.h"

namespace AmberRenderer::Resources
{
	class Mesh final : IMesh
	{
	public:
		Mesh(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices, uint32_t p_materialIndex);
		virtual ~Mesh() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual uint32_t GetVertexCount() override;
		virtual uint32_t GetIndexCount() override;

		uint32_t GetVAO() const { return m_vertexArray.GetID(); }
		uint32_t GetMaterialIndex() const;

	private:
		void InitializeBuffers(const std::vector<Geometry::Vertex>& p_vertices, const std::vector<uint32_t>& p_indices);

	private:
		const uint32_t m_vertexCount;
		const uint32_t m_indicesCount;
		uint32_t m_materialIndex;

		Buffers::VertexArray m_vertexArray;
		std::unique_ptr<Buffers::VertexBuffer> m_vertexBuffer;
		std::unique_ptr<Buffers::IndexBuffer> m_indexBuffer;
	};
}
