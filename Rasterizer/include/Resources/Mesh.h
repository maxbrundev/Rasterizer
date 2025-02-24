#pragma once

#include <memory>
#include <vector>

#include "Resources/IMesh.h"

#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexArray.h"
#include "Geometry/Vertex.h"

namespace Resources
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

		uint32_t GetVAO() const { return m_vertexArray.GetID(); /*return m_VAO;*/ }
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
