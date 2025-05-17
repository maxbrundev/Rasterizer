#include "AmberEditor/Resources/Shaders/ShadowMappingDepth.h"

AmberEditor::Resources::Shaders::ShadowMappingDepth::ShadowMappingDepth()
{
	RegisterUniform("u_Model", glm::mat4(1.0f));
	RegisterUniform("u_lightSpaceMatrix", glm::mat4(1.0f));
}

glm::vec4 AmberEditor::Resources::Shaders::ShadowMappingDepth::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model            = GetUniformAs<glm::mat4>("u_Model");
	const glm::mat4 u_lightSpaceMatrix = GetUniformAs<glm::mat4>("u_lightSpaceMatrix");

	const glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));

	const glm::vec4 FragPosLightSpace = u_lightSpaceMatrix * u_Model * glm::vec4(p_vertex.position, 1.0);

	SetVarying<glm::vec4>("v_FragPosLightSpace", FragPosLightSpace);

	return u_lightSpaceMatrix * u_Model * glm::vec4(p_vertex.position, 1.0);
}

glm::vec4 AmberEditor::Resources::Shaders::ShadowMappingDepth::FragmentPass()
{
	const glm::vec4 FragPosLightSpace = GetVaryingAs<glm::vec4>("v_FragPosLightSpace");

	glm::vec3 projCoords = glm::vec3(FragPosLightSpace) / FragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	const float currentDepth = projCoords.z;

	return glm::vec4(currentDepth, currentDepth, currentDepth, 1.0f);
}
