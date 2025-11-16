#pragma once

#include "OnyxEditor/Buffers/FrameBuffer.h"

#include "OnyxEditor/Rendering/Passes/ARenderPass.h"

#include "OnyxEditor/Resources/Material.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Passes
{
	class ShadowMappingPass : public ARenderPass
	{
	public:
		ShadowMappingPass(Renderer& p_renderer);
		~ShadowMappingPass() override;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;
		void Draw(RenderState p_pso, SceneSystem::AScene* p_scene) override;

	private:
		Resources::AShader* m_shadowMapDepthShader = nullptr;

		Resources::Material m_shadowMapDepthMaterial;
		Resources::Texture* m_shadowTexture;
		Buffers::FrameBuffer m_depthBuffer;
		uint16_t m_shadowMapSize;
	};
}