#include "AmberRenderer/Buffers/FrameBuffer.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"

AmberRenderer::Buffers::FrameBuffer::FrameBuffer(uint32_t p_width, uint32_t p_height)
{
	AmberGL::GenFramebuffers(1, &m_bufferID);
	AmberGL::GenTextures(1, &m_renderTexture);

	AmberGL::BindTexture(AGL_TEXTURE_2D, m_renderTexture);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_NEAREST);
	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	Bind();
	AmberGL::FramebufferTexture2D(AGL_FRAMEBUFFER, AGL_COLOR_ATTACHMENT, AGL_TEXTURE_2D, m_renderTexture, 0);
	Unbind();

	//Resize(p_width, p_height);
}

AmberRenderer::Buffers::FrameBuffer::~FrameBuffer()
{
	//TODO DeleteBuffers
}

void AmberRenderer::Buffers::FrameBuffer::Bind() const
{
	AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, m_bufferID);
}

void AmberRenderer::Buffers::FrameBuffer::Unbind() const
{
	AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, 0);
}

void AmberRenderer::Buffers::FrameBuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	//TODO:
}

uint32_t AmberRenderer::Buffers::FrameBuffer::GetID() const
{
	return m_bufferID;
}

uint32_t AmberRenderer::Buffers::FrameBuffer::GetTextureID() const
{
	return m_renderTexture;
}

const std::pair<uint16_t, uint16_t> AmberRenderer::Buffers::FrameBuffer::GetSize() const
{
	return m_size;
}
