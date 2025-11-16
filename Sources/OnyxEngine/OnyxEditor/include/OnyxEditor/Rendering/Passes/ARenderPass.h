#pragma once

#include <string>

#include <glm/glm.hpp>

#include "OnyxEditor/Entities/Camera.h"
#include "OnyxEditor/Rendering/PassOrder.h"
#include "OnyxEditor/Rendering/FrameInfo.h"
#include "OnyxEditor/Rendering/RenderState.h"
#include "OnyxEditor/Rendering/Debug/FrameDebuggerMacros.h"
#include "OnyxEditor/SceneSystem/AScene.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Passes
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


	protected:
		Renderer& m_renderer;
		bool m_isEnabled = true;
	};
}