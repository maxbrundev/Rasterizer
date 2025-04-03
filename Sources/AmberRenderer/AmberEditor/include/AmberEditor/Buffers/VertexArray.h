#pragma once

#include <cstdint>

namespace AmberEditor::Buffers
{
	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		uint32_t GetID() const;

	private:
		uint32_t m_bufferID;
		
	};
}
