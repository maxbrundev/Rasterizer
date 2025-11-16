#pragma once

#include "OnyxEditor/Rendering/Passes/ARenderPass.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Passes
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