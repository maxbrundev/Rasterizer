#include "AmberGL/SoftwareRenderer/Programs/StandardShader.h"

glm::vec4 AmberGL::SoftwareRenderer::Programs::StandardShader::VertexPass(const Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model      = GetUniformAs<glm::mat4>("u_Model");
	const glm::mat4 u_View       = GetUniformAs<glm::mat4>("u_View");
	const glm::mat4 u_Projection = GetUniformAs<glm::mat4>("u_Projection");

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	glm::vec3 normal = glm::mat3(glm::transpose(glm::inverse(u_Model))) * p_vertex.normal;
	SetVarying<glm::vec3>("v_Normal", normal);

	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	return  u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

const glm::vec3 LightPosition = glm::vec3(-9000.0f, 10000.0f, 11000.0f);
const glm::vec3 LightDiffuse = glm::vec3(1.0f);
const glm::vec3 LightAmbient = glm::vec3(0.3f);

AmberGL::Data::Color AmberGL::SoftwareRenderer::Programs::StandardShader::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	//return glm::vec4(((normal * 0.5f) + 0.5f), 1.0f);

	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 fragpos   = GetVaryingAs<glm::vec3>("v_FragPos");

	const glm::vec4 diffuse = Texture("u_DiffuseMap", texCoords);
	const glm::vec3 lambert = Lambert(fragpos, normal, LightPosition, LightDiffuse, LightAmbient);

	return diffuse * glm::vec4(lambert, 1.0f);
}
