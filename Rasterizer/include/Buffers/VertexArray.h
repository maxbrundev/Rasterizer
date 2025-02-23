#pragma once

#include <cstdint>

namespace Buffers
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void BindAttribPointer(const unsigned int p_size, const unsigned int p_type, const unsigned int p_normalized, const unsigned int p_stride, void* p_pointer);

		void Bind() const;
		void Unbind() const;

		uint32_t GetID() const;
		uint32_t index;

	private:
		uint32_t m_bufferID;
		
	};
}
