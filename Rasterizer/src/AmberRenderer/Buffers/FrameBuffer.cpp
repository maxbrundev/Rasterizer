#include "AmberRenderer/Buffers/FrameBuffer.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

AmberRenderer::Buffers::FrameBuffer::FrameBuffer(uint32_t p_width, uint32_t p_height)
{
	GLRasterizer::GenFramebuffers(1, &m_bufferID);
	GLRasterizer::GenTextures(1, &m_renderTexture);

	GLRasterizer::BindTexture(GLR_TEXTURE_2D, m_renderTexture);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MAG_FILTER, GLR_NEAREST);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MIN_FILTER, GLR_NEAREST);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

	Bind();
	GLRasterizer::FramebufferTexture2D(GLR_FRAMEBUFFER, GLR_COLOR_ATTACHMENT, GLR_TEXTURE_2D, m_renderTexture, 0);
	Unbind();

	//Resize(p_width, p_height);
}

AmberRenderer::Buffers::FrameBuffer::~FrameBuffer()
{
	//TODO DeleteBuffers
}

void AmberRenderer::Buffers::FrameBuffer::Bind() const
{
	GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, m_bufferID);
}

void AmberRenderer::Buffers::FrameBuffer::Unbind() const
{
	GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, 0);
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
