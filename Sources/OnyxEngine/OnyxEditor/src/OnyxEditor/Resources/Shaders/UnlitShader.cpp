#include "OnyxEditor/Resources/Shaders/UnlitShader.h"

OnyxEditor::Resources::Shaders::UnlitShader::UnlitShader()
{
	RegisterUniform("u_AlbedoColor", glm::vec3(1.0f, 1.0f, 1.0f));
}

glm::vec4 OnyxEditor::Resources::Shaders::UnlitShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(u_Model)));
	glm::vec3 worldNormal = glm::normalize(normalMatrix * p_vertex.normal);
	SetVarying<glm::vec3>("v_Normal", worldNormal);

	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	glm::vec3 worldTangent = glm::normalize(normalMatrix * p_vertex.tangent);
	SetVarying<glm::vec3>("v_Tangent", worldTangent);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::UnlitShader::FragmentPass()
{
	const glm::vec3 diffuseColor = GetUniformAs<glm::vec3>("u_AlbedoColor");

	return glm::vec4(diffuseColor, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> OnyxEditor::Resources::Shaders::UnlitShader::clone() const
{
	return std::make_unique<UnlitShader>(*this);
}
