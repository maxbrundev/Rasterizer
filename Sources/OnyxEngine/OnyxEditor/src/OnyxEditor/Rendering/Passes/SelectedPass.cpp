#include "OnyxEditor/Rendering/Passes/SelectedPass.h"

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/Rendering/Features/OutlineRenderFeature.h"

OnyxEditor::Rendering::Passes::SelectedPass::SelectedPass(Renderer& p_renderer) :
ARenderPass(p_renderer),
m_selectedModel(nullptr)
{
}

OnyxEditor::Rendering::Passes::SelectedPass::~SelectedPass()
{
}

void OnyxEditor::Rendering::Passes::SelectedPass::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void OnyxEditor::Rendering::Passes::SelectedPass::EndFrame()
{
}

void OnyxEditor::Rendering::Passes::SelectedPass::Draw(RenderState p_pso, SceneSystem::AScene* p_scene)
{
	auto& outline = m_renderer.GetFeature<Features::OutlineRenderFeature>();

	outline.DrawOutlines();
}

void OnyxEditor::Rendering::Passes::SelectedPass::SetSelectedModel(Resources::Model* p_model)
{
	m_selectedModel = p_model;

	auto& outline = m_renderer.GetFeature<Features::OutlineRenderFeature>();

	outline.AddModel(m_selectedModel);
}
