#pragma once

#include <algorithm>
#include <cstdint>

namespace AmberEditor::Buffers
{
	class FrameBuffer
	{
	public:
		FrameBuffer(uint32_t p_width, uint32_t p_height);
		~FrameBuffer();

		void Bind() const;
		void Unbind() const;

		void Resize(uint16_t p_width, uint16_t p_height);

		uint32_t GetID() const;
		uint32_t GetTextureID() const;

		void Blit(const FrameBuffer& p_destination, int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, bool p_blitColor, bool p_blitDepth, bool p_useLinearFilter)const;

		void BlitToScreen(int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, bool p_blitColor, bool p_blitDepth, bool p_useLinearFilter =false) const;

		const std::pair<uint16_t, uint16_t> GetSize() const;

	private:
		uint32_t m_bufferID = 0;
		uint32_t m_textureID = 0;

		std::pair<uint16_t, uint16_t> m_size;
	};
}
