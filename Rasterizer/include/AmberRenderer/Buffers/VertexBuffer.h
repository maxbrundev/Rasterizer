#pragma once
#include <cstdint>

namespace AmberRenderer::Buffers
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* p_data, size_t p_elements);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetID();

	private:
		uint32_t m_bufferID;
	};
}
