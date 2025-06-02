#pragma once

#include <glm/glm.hpp>

#include "AmberEditor/Rendering/Features/ARenderFeature.h"

namespace AmberEditor::Rendering
{
	class Renderer;
}

namespace AmberEditor::Rendering::Features
{
	class ShadowMappingFeature : public ARenderFeature
	{
	public:
		ShadowMappingFeature(Renderer& p_renderer);
		~ShadowMappingFeature() override = default;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;

		void OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable) override;
		void OnAfterDraw(const Entities::Drawable& p_drawable) override;

		void SetLightPosition(const glm::vec3& p_position);
		void UpdateLightMatrices();

	private:
		int16_t shadowMapResolution = 1024;

		glm::vec3 m_lightPosition = glm::vec3(5.0f, 6.0f, 5.0f);
		float m_nearPlane = 1.0f;
		float m_farPlane = 30.0f;

		glm::mat4 m_lightViewMatrix;
		glm::mat4 m_lightProjectionMatrix;
		glm::mat4 m_lightSpaceMatrix;
	};
}