#pragma once

#include <glm/glm.hpp>

#include "OnyxEditor/Rendering/Features/ARenderFeature.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Features
{
	class ShadowMappingRenderFeature : public ARenderFeature
	{
	public:
		ShadowMappingRenderFeature(Renderer& p_renderer);
		~ShadowMappingRenderFeature() override = default;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;

		void OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable) override;
		void OnAfterDraw(const Entities::Drawable& p_drawable) override;

		void SetLightPosition(const glm::vec3& p_position);
		void UpdateLightMatrices();

	private:
		glm::vec3 m_lightPosition = glm::vec3(5.0f, 6.0f, 5.0f);
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;

		glm::mat4 m_lightViewMatrix;
		glm::mat4 m_lightProjectionMatrix;
		glm::mat4 m_lightSpaceMatrix;
	};
}