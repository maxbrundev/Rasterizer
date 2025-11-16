#include "OnyxEditor/Rendering/Renderer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"
#include "OnyxEditor/Rendering/Passes/ARenderPass.h"
#include "OnyxEditor/Resources/Shaders/DebugShader.h"
#include "OnyxEditor/Resources/Loaders/ShaderLoader.h"

OnyxEditor::Rendering::Renderer::Renderer(Driver& p_driver, Context::IDisplay& p_display) :
ARenderer(p_driver),
m_display(p_display),
m_currentMode(Debug::EDebugViewMode::STANDARD)
{
	InitializeDebugMaterials();
	m_depthVisualizer = std::make_unique<Debug::DepthBufferVisualizer>();
}

void OnyxEditor::Rendering::Renderer::BeginFrame(const FrameInfo& p_frameDescriptor)
{
	ARenderer::BeginFrame(p_frameDescriptor);

	FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "Begin Frame")

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

void OnyxEditor::Rendering::Renderer::DrawFrame(SceneSystem::AScene* p_scene)
{
	if (m_currentMode == Debug::EDebugViewMode::DEPTH_BUFFER)
	{
		RenderDepthVisualization(p_scene);
	}
	else
	{
		RenderNormalPasses(p_scene);
	}

	/*RenderState defaultRenderState = m_driver.CreateRenderState();

	for (const auto& [order, pass] : m_passes)
	{
		if (m_frameDescriptor.OutputBuffer)
		{
			m_frameDescriptor.OutputBuffer->Bind();
		}
		SetViewport(0, 0, m_frameDescriptor.Width, m_frameDescriptor.Height);

		if (pass.second->IsEnabled())
		{
			FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "Begin Pass: " + pass.first);

			pass.second->Draw(defaultRenderState, p_scene);

			FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "End Pass: " + pass.first);
		}
	}*/
}

void OnyxEditor::Rendering::Renderer::EndFrame()
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

	FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "End Frame");
}

void OnyxEditor::Rendering::Renderer::Draw(RenderState p_pso, const Entities::Drawable& p_drawable)
{
	auto drawable = p_drawable;
	if (auto* debugMaterial = GetDebugMaterialForCurrentMode())
		drawable.Material = debugMaterial;

	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnBeforeDraw(p_pso, drawable);
		}
	}

	ARenderer::Draw(p_pso, drawable);

	for (const auto& [typeID, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnAfterDraw(drawable);
		}
	}
}

void OnyxEditor::Rendering::Renderer::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	m_driver.SetClearColor(p_red, p_green, p_blue, p_alpha);
}

void OnyxEditor::Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	AmberGL::SetSamples(p_samples);
}

void OnyxEditor::Rendering::Renderer::InitializeDebugMaterials()
{
	m_debugUVMaterial = std::make_unique<Resources::Material>();
	auto* uvShader = Resources::Loaders::ShaderLoader::Create<Resources::Shaders::DebugUVShader>();
	m_debugUVMaterial->SetShader(uvShader);
	m_debugUVMaterial->SetName("DebugUV");

	m_debugNormalsMaterial = std::make_unique<Resources::Material>();
	auto* normalsShader = Resources::Loaders::ShaderLoader::Create<Resources::Shaders::DebugNormalsShader>();
	m_debugNormalsMaterial->SetShader(normalsShader);
	m_debugNormalsMaterial->SetName("DebugWorldNormals");

	m_debugDepthMaterial = std::make_unique<Resources::Material>();
	auto* depthShader = Resources::Loaders::ShaderLoader::Create<Resources::Shaders::DebugDepthShader>();
	m_debugDepthMaterial->SetShader(depthShader);
	m_debugDepthMaterial->SetName("DebugDepth");
}

OnyxEditor::Resources::Material* OnyxEditor::Rendering::Renderer::GetDebugMaterialForCurrentMode() const
{
	switch (m_currentMode)
	{
	case Debug::EDebugViewMode::UV_COORDINATES:
		return m_debugUVMaterial.get();

	case Debug::EDebugViewMode::NORMALS:
		return m_debugNormalsMaterial.get();

	case Debug::EDebugViewMode::DEPTH_BUFFER:
		return m_debugDepthMaterial.get();

	default:
		return nullptr;
	}
}

void OnyxEditor::Rendering::Renderer::SetDebugViewMode(Debug::EDebugViewMode p_debugViewMode)
{
	m_currentMode = p_debugViewMode;

	switch (m_currentMode)
	{
	case Debug::EDebugViewMode::WIREFRAME:
		m_driver.SetRasterizationMode(Settings::EPolygonMode::LINE);
		break;

	default:
		m_driver.SetRasterizationMode(Settings::EPolygonMode::FILL);
		break;
	}
}

void OnyxEditor::Rendering::Renderer::RenderDepthVisualization(SceneSystem::AScene* p_scene)
{
	if (!m_depthVisualizer)
	{
		RenderNormalPasses(p_scene);
		return;
	}

	m_currentMode = Debug::EDebugViewMode::STANDARD;

	RenderNormalPasses(p_scene);

	m_depthVisualizer->CopyCurrentDepthBuffer(m_frameDescriptor.Width, m_frameDescriptor.Height);

	m_currentMode = Debug::EDebugViewMode::DEPTH_BUFFER;

	auto* debugMaterial = GetDebugMaterialForCurrentMode();

	if (debugMaterial->GetShader() && p_scene->GetCamera())
	{
		float nearPlane = p_scene->GetCamera()->GetNear();
		float farPlane = p_scene->GetCamera()->GetFar();

		debugMaterial->GetShader()->SetUniform("u_zNear", nearPlane);
		debugMaterial->GetShader()->SetUniform("u_zFar", farPlane);
		debugMaterial->GetShader()->SetUniform("colorImage", 0);

		AmberGL::ActiveTexture(0);
		AmberGL::BindTexture(AGL_TEXTURE_2D, m_depthVisualizer->GetDepthTexture());

		Clear(true, false, false, { 0.0f, 0.0f, 0.0f, 1.0f });
		RenderUnitQuad(debugMaterial);
	}
}

void OnyxEditor::Rendering::Renderer::RenderNormalPasses(SceneSystem::AScene* p_scene)
{
	RenderState defaultRenderState = m_driver.CreateRenderState();

	for (const auto& [order, pass] : m_passes)
	{
		if (m_frameDescriptor.OutputBuffer)
		{
			m_frameDescriptor.OutputBuffer->Bind();
		}
		SetViewport(0, 0, m_frameDescriptor.Width, m_frameDescriptor.Height);

		if (pass.second->IsEnabled())
		{
			FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "Begin Pass: " + pass.first);

			pass.second->Draw(defaultRenderState, p_scene);

			FRAME_DEBUG_RECORD_SET_RENDER_TARGET(*this, "End Pass: " + pass.first);
		}
	}
}

void OnyxEditor::Rendering::Renderer::DisplayPresent() const
{
	m_display.UpdateDisplayTexture(AmberGL::GetFrameBufferData(), AmberGL::GetFrameBufferRowSize());
	m_display.Present();
}

void OnyxEditor::Rendering::Renderer::DisplayClear() const
{
	m_display.Clear();
}
