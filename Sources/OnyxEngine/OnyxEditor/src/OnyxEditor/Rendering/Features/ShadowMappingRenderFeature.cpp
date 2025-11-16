#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"

#include <glm/gtc/matrix_transform.hpp>

#include "OnyxEditor/Resources/Material.h"
#include "OnyxEditor/Resources/Model.h"

OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::ShadowMappingRenderFeature(Renderer& p_renderer) :
ARenderFeature(p_renderer)
{
	UpdateLightMatrices();
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::BeginFrame(const FrameInfo& p_frameDescriptor)
{
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::OnBeforeDraw(RenderState& p_pso, const Entities::Drawable& p_drawable)
{
	auto& material = p_drawable.Material;

	if (material->IsShadowReceiver())
	{
		material->SetUniform("u_LightSpaceMatrix", m_lightSpaceMatrix);
		material->SetUniform("u_LightPosition", m_lightPosition);
		glm::vec3 lightTarget(0.0f, 0.0f, 0.0f);
		glm::vec3 lightDir = glm::normalize(lightTarget - m_lightPosition);
		material->SetUniform("u_LightDirection", lightDir);
	}
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::OnAfterDraw(const Entities::Drawable& p_drawable)
{
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::EndFrame()
{
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::SetLightPosition(const glm::vec3& p_position)
{
	m_lightPosition = p_position;
	UpdateLightMatrices();
}

void OnyxEditor::Rendering::Features::ShadowMappingRenderFeature::UpdateLightMatrices()
{
	glm::vec3 lightTarget(0.0f, 0.0f, 0.0f);
	m_lightViewMatrix = glm::lookAt(m_lightPosition, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));

	float orthoSize = 15.0f;
	m_lightProjectionMatrix = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, m_nearPlane, m_farPlane);

	m_lightSpaceMatrix = m_lightProjectionMatrix * m_lightViewMatrix;
}
