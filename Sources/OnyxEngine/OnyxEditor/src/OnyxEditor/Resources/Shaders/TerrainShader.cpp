#include "OnyxEditor/Resources/Shaders/TerrainShader.h"

#include <cmath>

OnyxEditor::Resources::Shaders::TerrainShader::TerrainShader() :
m_lightDirection(0.3f, 1.0f, 0.6f),
m_skyColor(131.0f / 255.0f, 188.0f / 255.0f, 243.0f / 255.0f)
{
	m_lightDirection = glm::normalize(m_lightDirection);
}

glm::vec4 OnyxEditor::Resources::Shaders::TerrainShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 worldPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0f));
	SetVarying<glm::vec3>("v_WorldPos", worldPos);

	// glm::vec3 normal = glm::mat3(glm::transpose(glm::inverse(u_Model))) * p_vertex.normal;
	SetVarying<glm::vec3>("v_Normal", p_vertex.normal /*normal*/);

	// Pass through texture coordinates (height is stored in x component)
	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	return u_Projection * u_View * glm::vec4(worldPos, 1.0f);
}

glm::vec4 OnyxEditor::Resources::Shaders::TerrainShader::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 worldPos = GetVaryingAs<glm::vec3>("v_WorldPos");
	const glm::vec3 cameraPos = GetUBOCameraPosition();

	float triangleHeight = texCoords.x;

	glm::vec3 terrainColor = m_colorLayers[0];

	for (int i = 0; i < m_heights.size(); i++)
	{
		if (triangleHeight > m_heights[i])
		{
			terrainColor = m_colorLayers[i + 1];
		}
		else
		{
			break;
		}
	}

	float lightIntensity = (glm::dot(normal, m_lightDirection) + 1.0f) * 0.5f;
	terrainColor *= lightIntensity;

	float depth = glm::length(worldPos - cameraPos);

	float aerialPerspectiveT = 1.0f - std::exp(-depth * atmosphereDensity);

	glm::vec3 finalColor = glm::mix(terrainColor, m_skyColor, aerialPerspectiveT);

	return glm::vec4(finalColor, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram>OnyxEditor::Resources::Shaders::TerrainShader::clone() const
{
	return std::make_unique<TerrainShader>(*this);
}
