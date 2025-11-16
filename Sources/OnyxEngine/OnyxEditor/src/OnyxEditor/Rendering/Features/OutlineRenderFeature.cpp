#include "OnyxEditor/Rendering/Features/OutlineRenderFeature.h"

#include <algorithm>

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/Managers/ShaderManager.h"
#include "OnyxEditor/Resources/Shaders/UnlitShader.h"
#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::Rendering::Features::OutlineRenderFeature::OutlineRenderFeature(Renderer& p_renderer) : ARenderFeature(p_renderer)
{
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();

	auto* unlitShader = shaderManager.GetShader<Resources::Shaders::UnlitShader>("Unlit");

	m_stencilFillMaterial.SetShader(unlitShader);
	m_stencilFillMaterial.SetName("StencilFill");
	m_stencilFillMaterial.SetDepthTest(false);
	m_stencilFillMaterial.SetColorWriting(false);

	m_outlineMaterial.SetShader(unlitShader);
	m_outlineMaterial.SetName("Outline");
	m_outlineMaterial.SetDepthTest(false);
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::EndFrame()
{
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable)
{
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::OnAfterDraw(const Entities::Drawable& p_drawable)
{
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawOutline(Resources::Model& p_model, const glm::vec4& p_color, float p_thickness)
{
	DrawStencilPass(p_model);
	DrawOutlinePass(p_model, p_color, p_thickness);
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawOutlines(const glm::vec4& p_color, float p_thickness)
{
	if (m_modelsToOutline.empty()) return;

	for (auto* model : m_modelsToOutline)
	{
		if (model)
		{
			DrawStencilPass(*model);
		}
	}

	for (auto* model : m_modelsToOutline)
	{
		if (model)
		{
			DrawOutlinePass(*model, p_color, p_thickness);
		}
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::AddModel(Resources::Model* p_model)
{
	if (p_model && std::find(m_modelsToOutline.begin(), m_modelsToOutline.end(), p_model) == m_modelsToOutline.end())
	{
		m_modelsToOutline.push_back(p_model);
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::RemoveModel(Resources::Model* p_model)
{
	auto it = std::find(m_modelsToOutline.begin(), m_modelsToOutline.end(), p_model);
	if (it != m_modelsToOutline.end())
	{
		m_modelsToOutline.erase(it);
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::ClearModels()
{
	m_modelsToOutline.clear();
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::SetDefaultOutlineColor(const glm::vec4& p_color)
{
	m_defaultOutlineColor = p_color;
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::SetDefaultOutlineThickness(float p_thickness)
{
	m_defaultOutlineThickness = p_thickness;
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawStencilPass(Resources::Model& p_model)
{
	RenderState stencilWriteState = m_renderer.GetDriver().CreateRenderState();

	stencilWriteState.StencilTest = true;
	stencilWriteState.StencilFunc = Settings::EComparisonOperand::ALWAYS;
	stencilWriteState.StencilRef = 1;
	stencilWriteState.StencilMask = 0xFF;
	stencilWriteState.StencilWriteMask = 0xFF;

	stencilWriteState.StencilFail = EStencilOp::REPLACE;
	stencilWriteState.StencilPassDepthFail = EStencilOp::REPLACE;
	stencilWriteState.StencilPassDepthPass = EStencilOp::REPLACE;

	auto& meshes = p_model.GetMeshes();
	
	for (auto* mesh : meshes)
	{
		Entities::Drawable drawable;
		drawable.Mesh = mesh;
		drawable.Material = &m_stencilFillMaterial;
		drawable.ModelMatrix = p_model.Transform.GetWorldMatrix();

		m_renderer.ARenderer::Draw(stencilWriteState, drawable);
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawOutlinePass(Resources::Model& p_model, const glm::vec4& p_color, float p_thickness)
{
	RenderState outlineState = m_renderer.GetDriver().CreateRenderState();

	outlineState.StencilTest = true;
	outlineState.StencilFunc = Settings::EComparisonOperand::NOT_EQUAL;
	outlineState.StencilRef = 1;
	outlineState.StencilMask = 1;
	outlineState.StencilWriteMask = 0x00;

	outlineState.StencilFail = EStencilOp::KEEP;
	outlineState.StencilPassDepthFail = EStencilOp::KEEP;
	outlineState.StencilPassDepthPass = EStencilOp::KEEP;

	m_outlineMaterial.SetUniform("u_AlbedoColor", glm::vec3(p_color.r, p_color.g, p_color.b));

	auto& meshes = p_model.GetMeshes();

	for (auto* mesh : meshes)
	{
		Entities::Drawable drawable;
		drawable.Mesh = mesh;
		drawable.Material = &m_outlineMaterial;

		glm::mat4 scaledMatrix = p_model.Transform.GetWorldMatrix();
		scaledMatrix = glm::scale(scaledMatrix, glm::vec3(p_thickness));
		drawable.ModelMatrix = scaledMatrix;

		m_renderer.ARenderer::Draw(outlineState, drawable);
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawModelToStencil(RenderState p_pso, Resources::Model& p_model, const glm::mat4& p_worldMatrix)
{
	auto& meshes = p_model.GetMeshes();
	auto& materials = p_model.GetMaterials();

	for (auto* mesh : meshes)
	{
		uint32_t materialIndex = mesh->GetMaterialIndex();
		if (materialIndex < MAX_MATERIAL_COUNT)
		{
			Resources::Material* material = materials.at(materialIndex);

			Entities::Drawable drawable;
			drawable.Mesh = mesh;
			drawable.Material = &m_stencilFillMaterial;
			drawable.ModelMatrix = p_worldMatrix;

			m_renderer.ARenderer::Draw(p_pso, drawable);
		}
	}
}

void OnyxEditor::Rendering::Features::OutlineRenderFeature::DrawModelOutline(RenderState p_pso, Resources::Model& p_model, const glm::mat4& p_worldMatrix)
{
	auto& meshes = p_model.GetMeshes();

	for (auto* mesh : meshes)
	{
		Entities::Drawable drawable;
		drawable.Mesh = mesh;
		drawable.Material = &m_outlineMaterial;
		drawable.ModelMatrix = p_worldMatrix;

		m_renderer.ARenderer::Draw(p_pso, drawable);
	}
}
