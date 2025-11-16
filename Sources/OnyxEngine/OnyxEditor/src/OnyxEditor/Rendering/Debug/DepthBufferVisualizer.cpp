#include "OnyxEditor/Rendering/Debug/DepthBufferVisualizer.h"

#include <iostream>

#include "OnyxEditor/Rendering/Features/ARenderFeature.h"

OnyxEditor::Rendering::Debug::DepthBufferVisualizer::DepthBufferVisualizer() : m_depthTextureID(0), m_initialized(false)
{
	Initialize();
}

OnyxEditor::Rendering::Debug::DepthBufferVisualizer::~DepthBufferVisualizer()
{
	Cleanup();
}

void OnyxEditor::Rendering::Debug::CopyDepthBuffer(uint32_t p_textureID, int p_x, int p_y, int p_width, int p_height)
{
	AmberGL::BindTexture(AGL_TEXTURE_2D, p_textureID);
	AmberGL::ReadBuffer(AGL_BACK); // Ensure we are reading from the back buffer.
	AmberGL::CopyTexImage2D(AGL_TEXTURE_2D, 0, AGL_DEPTH_COMPONENT, p_x, p_y, p_width, p_height, 0);
}

void OnyxEditor::Rendering::Debug::DepthBufferVisualizer::Initialize()
{
	if (m_initialized) 
		return;

	AmberGL::GenTextures(1, &m_depthTextureID);
	AmberGL::BindTexture(AGL_TEXTURE_2D, m_depthTextureID);

	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, AGL_CLAMP);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, AGL_CLAMP);

	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	m_initialized = true;
}

void OnyxEditor::Rendering::Debug::DepthBufferVisualizer::CopyCurrentDepthBuffer(uint32_t p_width, uint32_t p_height)
{
	if (!m_initialized)
	{
		Initialize();
	}

	CopyDepthBuffer(m_depthTextureID, 0, 0, p_width, p_height);
}

void OnyxEditor::Rendering::Debug::DepthBufferVisualizer::Cleanup()
{
	if (m_depthTextureID != 0)
	{
		AmberGL::DeleteTextures(1, &m_depthTextureID);
		m_depthTextureID = 0;
	}

	m_initialized = false;
}
