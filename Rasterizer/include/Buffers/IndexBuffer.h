#pragma once

#include <cstdint>

namespace Buffers
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* p_data, size_t p_elements);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetID() const;

	private:
		uint32_t m_bufferID;
	};
}
