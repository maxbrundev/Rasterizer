#pragma once

#include <string>

#include "AmberEditor/Entities/Drawable.h"

#include "AmberEditor/Rendering/FrameInfo.h"
#include "AmberEditor/Rendering/RenderState.h"

namespace AmberEditor::Rendering
{
	class Renderer;
}

namespace AmberEditor::Rendering::Features
{
	class ARenderFeature
	{
	public:
		ARenderFeature(Renderer& p_renderer);
		virtual ~ARenderFeature() = default;

		virtual void BeginFrame(const FrameInfo& p_frameDescriptor) = 0;
		virtual void EndFrame() = 0;

		virtual void OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable) = 0;
		virtual void OnAfterDraw(const Entities::Drawable& p_drawable) = 0;

		void SetEnabled(bool p_value);
		bool IsEnabled() const;

	public:
		const std::string Name;

	protected:
		Renderer& m_renderer;
		bool m_isEnabled = true;
	};
}