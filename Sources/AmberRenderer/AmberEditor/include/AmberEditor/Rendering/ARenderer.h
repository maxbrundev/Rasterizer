#pragma once

#include <glm/glm.hpp>

#include "AmberEditor/Entities/Drawable.h"

#include "AmberEditor/Rendering/Driver.h"
#include "AmberEditor/Rendering/FrameInfo.h"
#include "AmberEditor/Rendering/RenderState.h"

#include "AmberEditor/Resources/IMesh.h"
#include "AmberEditor/Resources/Texture.h"

#include "AmberEditor/SceneSystem/AScene.h"

namespace AmberEditor::Rendering
{
	class ARenderer
	{
	public:
		ARenderer(Driver& p_driver);
		virtual ~ARenderer();

		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;

		virtual void Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer, const glm::vec4& p_color = glm::vec4(0.0f));

		virtual void BeginFrame(const FrameInfo& p_frameDescriptor);
		virtual void DrawFrame(SceneSystem::AScene* p_scene) = 0;
		virtual void EndFrame();

		virtual void Draw(RenderState p_renderState, const Entities::Drawable& p_drawable);

	private:
		void InitializeResources();

	protected:
		Driver& m_driver;
		Resources::Texture* m_emptyTexture;
		IMesh* m_unitQuad;
		FrameInfo m_frameDescriptor;
	};
}
