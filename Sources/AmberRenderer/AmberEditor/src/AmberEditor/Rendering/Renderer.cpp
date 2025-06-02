#include "AmberEditor/Rendering/Renderer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "AmberEditor/Rendering/Passes/ARenderPass.h"

AmberEditor::Rendering::Renderer::Renderer(Driver& p_driver, Context::IDisplay& p_display) :
ARenderer(p_driver),
m_display(p_display)
{
	//p_window.ResizeEvent.AddListener(std::bind(&Renderer::OnResize, this, std::placeholders::_1, std::placeholders::_2));
}

void AmberEditor::Rendering::Renderer::BeginFrame(const FrameInfo& p_frameDescriptor)
{
	ARenderer::BeginFrame(p_frameDescriptor);

	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->BeginFrame(p_frameDescriptor);
		}
	}

	for (const auto& [oder, pass] : m_passes)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->BeginFrame(p_frameDescriptor);
		}
	}
}

void AmberEditor::Rendering::Renderer::DrawFrame(SceneSystem::AScene* p_scene)
{
	RenderState defaultRenderState = m_driver.CreateRenderState();

	for (const auto& [oder, pass] : m_passes)
	{

		if (m_frameDescriptor.OutputBuffer)
		{
			m_frameDescriptor.OutputBuffer->Bind();
		}

		SetViewport(0, 0, m_frameDescriptor.Width, m_frameDescriptor.Height);

		if (pass.second->IsEnabled())
		{
			pass.second->Draw(defaultRenderState, p_scene);
		}
	}
}

void AmberEditor::Rendering::Renderer::EndFrame()
{

	for (const auto& [typeID, pass] : m_passes)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->EndFrame();
		}
	}

	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->EndFrame();
		}
	}

	ARenderer::EndFrame();

}

void AmberEditor::Rendering::Renderer::Draw(RenderState p_pso, const Entities::Drawable& p_drawable)
{
	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnBeforeDraw(p_pso, p_drawable);
		}
	}

	ARenderer::Draw(p_pso, p_drawable);

	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnAfterDraw(p_drawable);
		}
	}
}

void AmberEditor::Rendering::Renderer::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	m_driver.SetClearColor(p_red, p_green, p_blue, p_alpha);
}

void AmberEditor::Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	AmberGL::SetSamples(p_samples);
}

void AmberEditor::Rendering::Renderer::OnResize(uint16_t p_width, uint16_t p_height)
{
	//m_textureBuffer.Resize(p_width, p_height);
	//m_depthBuffer.Resize(p_width, p_height);

	//SDL_DestroyTexture(m_sdlTexture);

	//m_sdlTexture = SDL_CreateTexture(m_driver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(GLRasterizer::GetTextureBuffer().GetWidth()), static_cast<int>(GLRasterizer::GetTextureBuffer().GetHeight()));
}


void AmberEditor::Rendering::Renderer::DisplayPresent() const
{
	m_display.UpdateDisplayTexture(AmberGL::GetFrameBufferData(), AmberGL::GetFrameBufferRowSize());
	m_display.Present();
}

void AmberEditor::Rendering::Renderer::DisplayClear() const
{
	m_display.Clear();
}
