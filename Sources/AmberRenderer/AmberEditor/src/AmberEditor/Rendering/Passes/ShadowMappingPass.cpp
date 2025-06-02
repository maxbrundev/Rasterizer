#include "AmberEditor/Rendering/Passes/ShadowMappingPass.h"

#include "AmberEditor/Managers/ShaderManager.h"

#include "AmberEditor/Rendering/Renderer.h"
#include "AmberEditor/Rendering/Features/ShadowMappingFeature.h"

#include "AmberEditor/Resources/Loaders/TextureLoader.h"
#include "AmberEditor/Resources/Shaders/ShadowMappingDepth.h"

#include "AmberEditor/SceneSystem/AScene.h"

#include "AmberEditor/Tools/Globals/ServiceLocator.h"

AmberEditor::Rendering::Passes::ShadowMappingPass::ShadowMappingPass(Renderer& p_renderer) :
ARenderPass(p_renderer),
m_shadowMapSize(1024),
m_depthBuffer(1024, 1024)
{
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();

	m_shadowMapDepthShader = shaderManager.GetShader<Resources::Shaders::ShadowMappingDepth>("ShadowMapDepth");
	m_shadowMapDepthMaterial.SetShader(m_shadowMapDepthShader);
	m_shadowTexture = Resources::Loaders::TextureLoader::CreateDepth(nullptr, m_shadowMapSize, m_shadowMapSize, Settings::ETextureFilteringMode::NEAREST, Settings::ETextureFilteringMode::NEAREST);

	m_depthBuffer.Bind();
	m_depthBuffer.Attach(m_shadowTexture, Settings::EFramebufferAttachment::DEPTH);
	m_depthBuffer.Unbind();
}

AmberEditor::Rendering::Passes::ShadowMappingPass::~ShadowMappingPass()
{
	Resources::Loaders::TextureLoader::Destroy(m_shadowTexture);
}

void AmberEditor::Rendering::Passes::ShadowMappingPass::BeginFrame(const FrameInfo& p_frameDescriptor)
{

	auto& shadowMappingFeature = m_renderer.GetFeature<Features::ShadowMappingFeature>();
	shadowMappingFeature.UpdateLightMatrices();
}

void AmberEditor::Rendering::Passes::ShadowMappingPass::EndFrame()
{
}

void AmberEditor::Rendering::Passes::ShadowMappingPass::Draw(RenderState p_pso, SceneSystem::AScene* p_scene)
{
	m_depthBuffer.Bind();
	
	m_renderer.SetViewport(0, 0, 1024, 1024);

	m_renderer.Clear(false, true, false);

	auto& models = p_scene->GetModels();

	for (auto* model : models)
	{
		auto& materials = model->GetMaterials();

		for (auto mesh : model->GetMeshes())
		{
			uint32_t materialIndex = mesh->GetMaterialIndex();

			if (materialIndex < MAX_MATERIAL_COUNT)
			{
				Resources::Material* material = materials.at(materialIndex);

				if (material->HasUniform("u_DepthMap"))
				{
					material->SetUniform("u_DepthMap", m_shadowTexture);
				}

				glm::mat4 modelMatrix = material->GetUniform<glm::mat4>("u_Model");

				m_shadowMapDepthMaterial.SetUniform("u_Model", modelMatrix);

				Entities::Drawable drawable;
				drawable.Mesh = mesh;
				drawable.Material = &m_shadowMapDepthMaterial;

				m_renderer.Draw(p_pso, drawable);
			}
		}
	}

	m_depthBuffer.Unbind();
}
