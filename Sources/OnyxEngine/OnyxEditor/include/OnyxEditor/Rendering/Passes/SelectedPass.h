#pragma once

#pragma once

#include "OnyxEditor/Rendering/Passes/ARenderPass.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Passes
{
	class SelectedPass : public ARenderPass
	{
	public:
		SelectedPass(Renderer& p_renderer);
		~SelectedPass() override;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;
		void Draw(RenderState p_pso, SceneSystem::AScene* p_scene) override;

		void SetSelectedModel(Resources::Model* p_model);

	private:
		Resources::Model* m_selectedModel;
	};
}