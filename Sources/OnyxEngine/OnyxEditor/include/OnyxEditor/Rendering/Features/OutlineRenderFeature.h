#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "OnyxEditor/Rendering/Features/ARenderFeature.h"
#include "OnyxEditor/Resources/Material.h"
#include "OnyxEditor/Resources/Model.h"

namespace OnyxEditor::Rendering
{
	class Renderer;
}

namespace OnyxEditor::Rendering::Features
{
	class OutlineRenderFeature : public ARenderFeature
	{
	public:
		OutlineRenderFeature(Renderer& p_renderer);
		~OutlineRenderFeature() override = default;

		void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		void EndFrame() override;
		void OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable) override;
		void OnAfterDraw(const Entities::Drawable& p_drawable) override;

		void DrawOutline(Resources::Model& p_model, const glm::vec4& p_color = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f), float p_thickness = 1.05f);

		void DrawOutlines(const glm::vec4& p_color = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f), float p_thickness = 1.05f);

		void AddModel(Resources::Model* p_model);
		void RemoveModel(Resources::Model* p_model);
		void ClearModels();

		bool HasModels() const { return !m_modelsToOutline.empty(); }

		void SetDefaultOutlineColor(const glm::vec4& p_color);
		void SetDefaultOutlineThickness(float p_thickness);

	private:
		void DrawStencilPass(Resources::Model& p_model);
		void DrawOutlinePass(Resources::Model& p_model, const glm::vec4& p_color, float p_thickness);
		void DrawModelToStencil(RenderState p_pso, Resources::Model& p_model, const glm::mat4& p_worldMatrix);
		void DrawModelOutline(RenderState p_pso, Resources::Model& p_model, const glm::mat4& p_worldMatrix);

	private:
		Resources::Material m_stencilFillMaterial;
		Resources::Material m_outlineMaterial;

		std::vector<Resources::Model*> m_modelsToOutline;

		glm::vec4 m_defaultOutlineColor = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);
		float m_defaultOutlineThickness = 1.05f;
	};
}