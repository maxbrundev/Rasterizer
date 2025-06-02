#include "AmberEditor/Rendering/Passes/OpaquePass.h"

#include "AmberEditor/Rendering/Renderer.h"
#include "AmberEditor/Rendering/Features/ShadowMappingFeature.h"

#include "AmberEditor/SceneSystem/AScene.h"

AmberEditor::Rendering::Passes::OpaquePass::OpaquePass(Renderer& p_renderer) :
ARenderPass(p_renderer)
{
}

void AmberEditor::Rendering::Passes::OpaquePass::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void AmberEditor::Rendering::Passes::OpaquePass::EndFrame()
{
}

void AmberEditor::Rendering::Passes::OpaquePass::Draw(RenderState p_pso, SceneSystem::AScene* p_scene)
{
	auto& models = p_scene->GetModels();

	for (auto model : models)
	{
		auto& materials = model->GetMaterials();

		for (auto mesh : model->GetMeshes())
		{
			uint32_t materialIndex = mesh->GetMaterialIndex();

			if (materialIndex < MAX_MATERIAL_COUNT)
			{
				Resources::Material* material = materials.at(materialIndex);

				Entities::Drawable drawable;
				drawable.Mesh = mesh;
				drawable.Material = material;

				m_renderer.Draw(p_pso, drawable);
			}
		}
	}
}
