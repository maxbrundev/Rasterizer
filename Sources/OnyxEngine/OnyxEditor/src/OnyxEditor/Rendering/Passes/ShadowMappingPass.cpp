#include "OnyxEditor/Rendering/Passes/ShadowMappingPass.h"

#include "OnyxEditor/Managers/ShaderManager.h"

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"
#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"

#include "OnyxEditor/Resources/Loaders/TextureLoader.h"
#include "OnyxEditor/Resources/Shaders/ShadowMappingDepth.h"

#include "OnyxEditor/SceneSystem/AScene.h"

#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::Rendering::Passes::ShadowMappingPass::ShadowMappingPass(Renderer& p_renderer) :
ARenderPass(p_renderer),
m_shadowMapSize(2048),
m_depthBuffer(2048, 2048)
{
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();

	m_shadowMapDepthShader = shaderManager.GetShader<Resources::Shaders::ShadowMappingDepth>("ShadowMapDepth");
	m_shadowMapDepthMaterial.SetShader(m_shadowMapDepthShader);
    m_shadowMapDepthMaterial.SetName("ShadowMapDepth");
	m_shadowTexture = Resources::Loaders::TextureLoader::CreateDepth(nullptr, m_shadowMapSize, m_shadowMapSize, Settings::ETextureFilteringMode::NEAREST, Settings::ETextureFilteringMode::NEAREST);

	m_depthBuffer.Bind();
	m_depthBuffer.Attach(m_shadowTexture, Settings::EFramebufferAttachment::DEPTH);
	m_depthBuffer.Unbind();
}

OnyxEditor::Rendering::Passes::ShadowMappingPass::~ShadowMappingPass()
{
	Resources::Loaders::TextureLoader::Destroy(m_shadowTexture);
}

void OnyxEditor::Rendering::Passes::ShadowMappingPass::BeginFrame(const FrameInfo& p_frameDescriptor)
{
	auto& shadowMappingFeature = m_renderer.GetFeature<Features::ShadowMappingRenderFeature>();
	shadowMappingFeature.UpdateLightMatrices();
}

void OnyxEditor::Rendering::Passes::ShadowMappingPass::EndFrame()
{
}

void OnyxEditor::Rendering::Passes::ShadowMappingPass::Draw(RenderState p_pso, SceneSystem::AScene* p_scene)
{
	m_depthBuffer.Bind();

	m_renderer.SetViewport(0, 0, m_shadowMapSize, m_shadowMapSize);

	m_renderer.Clear(false, true, false);

	auto& models = p_scene->GetModels();
	int modelIndex = 0;

	for (auto* model : models)
	{
		auto& materials = model->GetMaterials();

		for (auto mesh : model->GetMeshes())
		{
			uint32_t materialIndex = mesh->GetMaterialIndex();

			if (materialIndex < MAX_MATERIAL_COUNT)
			{
				Resources::Material* material = materials.at(materialIndex);

				material->SetUniform("u_DepthMap", m_shadowTexture);

				Entities::Drawable drawable;
				drawable.Mesh = mesh;
				drawable.Material = &m_shadowMapDepthMaterial;
				drawable.ModelMatrix = model->Transform.GetWorldMatrix();
				
				m_renderer.Draw(p_pso, drawable);

				modelIndex++;
			}
		}
	}

	m_depthBuffer.Unbind();
}
