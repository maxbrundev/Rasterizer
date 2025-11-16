#include "OnyxEditor/Resources/Shaders/StandardShader.h"

OnyxEditor::Resources::Shaders::StandardShader::StandardShader()
{
	RegisterUniform("u_DiffuseMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 0 });
	RegisterUniform("u_LightDirection", glm::vec3(0.0f, -1.0f, 0.0f));
	RegisterUniform("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	RegisterUniform("u_LightIntensity", 1.0f);
	RegisterUniform("u_AmbientColor", glm::vec3(0.2f, 0.2f, 0.2f));
	RegisterUniform("u_AlbedoColor", glm::vec3(1.0f, 1.0f, 1.0f));
}

glm::vec4 OnyxEditor::Resources::Shaders::StandardShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
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

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::StandardShader::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");

	const glm::vec3 lightDirection = GetUniformAs<glm::vec3>("u_LightDirection");
	const glm::vec3 lightColor = GetUniformAs<glm::vec3>("u_LightColor");
	const float lightIntensity = GetUniformAs<float>("u_LightIntensity");
	const glm::vec3 ambientColor = GetUniformAs<glm::vec3>("u_AmbientColor");
	const glm::vec3 albedoColor = GetUniformAs<glm::vec3>("u_AlbedoColor");

	const glm::vec4 diffuseColor = Texture("u_DiffuseMap", texCoords);

	const glm::vec3 lightDir = glm::normalize(-lightDirection);

	const glm::vec3 albedo = glm::vec3(diffuseColor) * albedoColor;

	const float diffuseFactor = glm::max(glm::dot(normal, lightDir), 0.0f);
	const glm::vec3 diffuse = lightColor * diffuseFactor * lightIntensity;

	const glm::vec3 finalColor = (ambientColor + diffuse) * albedo;

	return glm::vec4(glm::clamp(finalColor, 0.0f, 1.0f), diffuseColor.a);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram>OnyxEditor::Resources::Shaders::StandardShader::clone() const
{
	return std::make_unique<StandardShader>(*this);
}

