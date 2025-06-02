#pragma once

#include <string>

#include <glm/glm.hpp>

#include "AmberEditor/Entities/Camera.h"
#include "AmberEditor/Rendering/PassOrder.h"
#include "AmberEditor/Rendering/FrameInfo.h"
#include "AmberEditor/Rendering/RenderState.h"
#include "AmberEditor/SceneSystem/AScene.h"

namespace AmberEditor::Rendering
{
	class Renderer;
}

namespace AmberEditor::Rendering::Passes
{
	class ARenderPass
	{
	public:
		ARenderPass(Renderer& p_renderer);
		virtual ~ARenderPass() = default;

		virtual void BeginFrame(const FrameInfo& p_frameDescriptor) = 0;
		virtual void EndFrame() = 0;
		virtual void Draw(RenderState p_pso, SceneSystem::AScene* p_scene) = 0;

		void SetEnabled(bool p_value);
		bool IsEnabled() const;

	protected:
		Renderer& m_renderer;
		bool m_isEnabled = true;
	};
}