#include "AmberEditor/Buffers/FrameBuffer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "AmberEditor/Tools/Utils/Enum.h"

AmberEditor::Buffers::FrameBuffer::FrameBuffer(uint32_t p_width, uint32_t p_height)
{
	AmberGL::GenFrameBuffers(1, &m_bufferID);
}

AmberEditor::Buffers::FrameBuffer::~FrameBuffer()
{
	AmberGL::DeleteFrameBuffer(1, &m_bufferID);
}

void AmberEditor::Buffers::FrameBuffer::Bind() const
{
	AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, m_bufferID);
}

void AmberEditor::Buffers::FrameBuffer::Unbind() const
{
	AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, 0);
}

void AmberEditor::Buffers::FrameBuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	//TODO:
}

uint32_t AmberEditor::Buffers::FrameBuffer::GetID() const
{
	return m_bufferID;
}

void AmberEditor::Buffers::FrameBuffer::Blit(const FrameBuffer& p_destination, int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, bool p_blitColor, bool p_blitDepth, bool p_useLinearFilter) const
{
	if (p_sourceX1 < 0) p_sourceX1 = m_size.first;
	if (p_sourceY1 < 0) p_sourceY1 = m_size.second;
	if (p_destinationX1 < 0) p_destinationX1 = p_destination.GetSize().first;
	if (p_destinationY1 < 0) p_destinationY1 = p_destination.GetSize().second;
	
	uint8_t mask = 0;
	if (p_blitColor) mask |= AGL_COLOR_BUFFER_BIT;
	if (p_blitDepth) mask |= AGL_DEPTH_BUFFER_BIT;

	uint8_t filter = p_useLinearFilter ? AGL_LINEAR : AGL_NEAREST;

	AmberGL::BlitFrameBuffer(m_bufferID, p_destination.GetID(),
		p_sourceX0, p_sourceY0, p_sourceX1, p_sourceY1,
		p_destinationX0, p_destinationY0, p_destinationX1, p_destinationY1,
		mask, filter);
}

void AmberEditor::Buffers::FrameBuffer::Attach(const Resources::Texture* p_texture, Rendering::Settings::EFramebufferAttachment p_attachment) const
{
	AmberGL::FrameBufferTexture2D(AGL_FRAMEBUFFER, GetEnumValue<uint16_t>(p_attachment), AGL_TEXTURE_2D, p_texture->ID, 0);
}

void AmberEditor::Buffers::FrameBuffer::BlitToScreen(int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, bool p_blitColor, bool p_blitDepth, bool p_useLinearFilter) const
{
	if (p_sourceX1 < 0) p_sourceX1 = m_size.first;
	if (p_sourceY1 < 0) p_sourceY1 = m_size.second;

	uint8_t mask = 0;
	if (p_blitColor) mask |= AGL_COLOR_BUFFER_BIT;
	if (p_blitDepth) mask |= AGL_DEPTH_BUFFER_BIT;

	uint8_t filter = p_useLinearFilter ? AGL_LINEAR : AGL_NEAREST;

	AmberGL::BlitFrameBuffer(m_bufferID, 0,
		p_sourceX0, p_sourceY0, p_sourceX1, p_sourceY1,
		p_destinationX0, p_destinationY0, p_destinationX1, p_destinationY1,
		mask, filter);
}

const std::pair<uint16_t, uint16_t> AmberEditor::Buffers::FrameBuffer::GetSize() const
{
	return m_size;
}
