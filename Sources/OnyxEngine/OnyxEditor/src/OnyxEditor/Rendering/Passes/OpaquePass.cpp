#include "OnyxEditor/Rendering/Passes/OpaquePass.h"

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"
#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"

#include "OnyxEditor/SceneSystem/AScene.h"

OnyxEditor::Rendering::Passes::OpaquePass::OpaquePass(Renderer& p_renderer) : ARenderPass(p_renderer)
{
}

void OnyxEditor::Rendering::Passes::OpaquePass::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void OnyxEditor::Rendering::Passes::OpaquePass::EndFrame()
{
}

void OnyxEditor::Rendering::Passes::OpaquePass::Draw(RenderState p_pso, SceneSystem::AScene* p_scene)
{
	auto& models = p_scene->GetModels();
	int modelIndex = 0;

	for (auto model : models)
	{
		auto& materials = model->GetMaterials();
		auto& meshes = model->GetMeshes();

		for (auto mesh : meshes)
		{
			uint32_t materialIndex = mesh->GetMaterialIndex();
			if (materialIndex < MAX_MATERIAL_COUNT)
			{
				Resources::Material* material = materials.at(materialIndex);

				Entities::Drawable drawable;
				drawable.Mesh = mesh;
				drawable.Material = material;
				drawable.ModelMatrix = model->Transform.GetWorldMatrix();
				
				m_renderer.Draw(p_pso, drawable);
				modelIndex++;
			}
		}
	}
}
