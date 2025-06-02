#pragma once

#include "AmberEditor/Rendering/Passes/ARenderPass.h"

namespace AmberEditor::Rendering
{
	class Renderer;
}

namespace AmberEditor::Rendering::Passes
{
	class OpaquePass : public ARenderPass
	{
	public:
		OpaquePass(Renderer& p_renderer);
		~OpaquePass() override = default;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;
		void Draw(RenderState p_pso, SceneSystem::AScene* p_scene) override;
	};
}