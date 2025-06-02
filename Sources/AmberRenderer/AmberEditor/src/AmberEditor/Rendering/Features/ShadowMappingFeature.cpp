#include "AmberEditor/Rendering/Features/ShadowMappingFeature.h"

#include <glm/gtc/matrix_transform.hpp>

#include "AmberEditor/Resources/Material.h"
#include "AmberEditor/Resources/Model.h"

AmberEditor::Rendering::Features::ShadowMappingFeature::ShadowMappingFeature(Renderer& p_renderer) :
ARenderFeature(p_renderer)
{
	UpdateLightMatrices();
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable)
{
	auto& material = p_drawable.Material;

	if (material->IsShadowReceiver())
	{
		if (material->HasUniform("u_lightSpaceMatrix"))
		{
			material->SetUniform("u_lightSpaceMatrix", m_lightSpaceMatrix);
		}

		if (material->HasUniform("u_lightPos"))
		{
			material->SetUniform("u_lightPos", m_lightPosition);
		}
	}
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::OnAfterDraw(const Entities::Drawable& p_drawable)
{
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::EndFrame()
{
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::SetLightPosition(const glm::vec3& p_position)
{
	m_lightPosition = p_position;
	UpdateLightMatrices();
}

void AmberEditor::Rendering::Features::ShadowMappingFeature::UpdateLightMatrices()
{
	glm::vec3 lightTarget(0.0f, 0.0f, 0.0f);
	m_lightViewMatrix = glm::lookAt(m_lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));

	float orthoSize = 15.0f;
	m_lightProjectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, m_nearPlane, m_farPlane);

	m_lightSpaceMatrix = m_lightProjectionMatrix * m_lightViewMatrix;
}
