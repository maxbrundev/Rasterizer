#pragma once

#include "AmberEditor/Buffers/FrameBuffer.h"

#include "AmberEditor/Rendering/Passes/ARenderPass.h"

#include "AmberEditor/Resources/Material.h"

namespace AmberEditor::Rendering
{
	class Renderer;
}

namespace AmberEditor::Rendering::Passes
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